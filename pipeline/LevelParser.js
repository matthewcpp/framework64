const Bounding = require("./gltf/Bounding");
const GLTFLoader = require("./gltf/GLTFLoader");
const MaterialBundle = require("./gltf/MaterialBundle");
const N64Node = require("./gltf/Node");
const Scene = require("./gltf/Scene");

class LevelParser {
    gltfData = null;
    layerMap = null;
    scenes = [];
    collisonMeshMap = new Map();
    transformCustomBoundingBoxIndex = -1;

    /** Index into the gltf node array that represents current scne mesh collider root*/
    _meshColliderNodeIndex = GLTFLoader.InvalidNodeIndex;

    _meshes = [];

    async parse(gltfPath, layerMap) {
        this._reset();

        this.layerMap = layerMap;
        await this.gltfData.loadFile(gltfPath);

        const gltf = this.gltfData.gltf;

        const rootNode = gltf.scenes[gltf.scene];

        for (const sceneIndex of rootNode.nodes) {
            const scene = this._parseScene(sceneIndex);

            if (scene) {
                this.scenes.push(scene);
            }
        }
    }

    _findChildNodeStartingWith(gltfNode, str) {
        if (!Object.hasOwn(gltfNode, "children")) {
            return null;
        }

        for (const nodeIndex of gltfNode.children) {
            const node = this.gltfData.gltf.nodes[nodeIndex];

            if (!Object.hasOwn(node, "name")) {
                continue;
            }

            if (node.name.startsWith(str)) {
                return node;
            }
        }

        return null;
    }

    _parseCollisionMeshes(scene, gltfRootNode) {
        const gltfCollidersNodeRoot = this._findChildNodeStartingWith(gltfRootNode, "Colliders");
        if (!gltfCollidersNodeRoot) {
            return null;
        }

        for (const child_index of gltfCollidersNodeRoot.children) {
            const gltfNode = this.gltfData.gltf.nodes[child_index];

            if (!Object.hasOwn(gltfNode, "mesh")) {
                continue;
            }

            if (this.collisonMeshMap.has(gltfNode.name)) {
                throw new Error(`duplicate collion mesh name: ${gltfNode.name}`)
            }

            this.collisonMeshMap.set(gltfNode.name, scene.collisionMeshes.length);
            scene.collisionMeshes.push(this.gltfData.meshes[gltfNode.mesh]);
        }
    }

    _parseSceneExtras(scene, sceneGltfNode) {
        // TODO: Add more supported Extra fields

        if (!Object.hasOwn(sceneGltfNode, "extras")) {
            return;
        }

        const extras = sceneGltfNode["extras"];

        if (Object.hasOwn(extras, "extraColliders")) {
            const extraColliders = parseInt(extras["extraColliders"]);

            if (!isNaN(extraColliders)) {
                scene.colliderCount += extraColliders;
            }
        }

        if (Object.hasOwn(extras, "extraMeshInstances")) {
            const extraMeshInstances = parseInt(extras["extraMeshInstances"]);

            if (!isNaN(extraMeshInstances)) {
                scene.meshInstanceCount += extraMeshInstances;
            }
        }

        if (Object.hasOwn(extras, "extraMeshes")) {
            const extraMeshes = parseInt(extras["extraMeshes"]);

            if (!isNaN(extraMeshes)) {
                scene.extraMeshCount += extraMeshes;
            }
        }

        if (Object.hasOwn(extras, "extraNodes")) {
            const extraNodes = parseInt(extras["extraNodes"]);

            if (!isNaN(extraNodes)) {
                scene.extraNodeCount += extraNodes;
            }
        }
    }

    _parseNode(scene, parentNode, gltfNode) {
        const node = this._createAndAddNode(scene, parentNode);
        node.name = parentNode === null ? "root" : gltfNode.name; 

        const gltf = this.gltfData.gltf;

        this._extractNodeTransform(gltfNode, node);
        this._parseNodeExtras(scene, gltfNode, node);
        this._parseNodeMesh(scene, gltfNode, node);

        if (Object.hasOwn(gltfNode, "children")) {
            for (const childIndex of gltfNode.children) {
                const gltfChildNode = gltf.nodes[childIndex];
                const nodeHasName = Object.hasOwn(gltfChildNode, "name");

                // Note: currently custom bounding boxes are set using child nodes beginning with _boundingBox.
                // We do not want to parse that node in this case
                if (nodeHasName && gltfChildNode.name.startsWith("_boundingBox")) {
                    this._getCustomBoundingBoxForNode(scene, gltfChildNode, node);
                    continue;
                }

                this._parseNode(scene, node, gltfChildNode);
            }
        }

        if (node.collider != N64Node.NoCollider) {
            scene.colliderCount += 1;
        }
    }

    _createAndAddNode(scene, parentNode) {
        const node = new N64Node(scene.nodes.length, parentNode);
        scene.nodes.push(node);

        if (parentNode) {
            parentNode.childNodes.push(node);
        }

        return node;
    }

    _parseScene(sceneRootNodeIndex) {
        const gltf = this.gltfData.gltf;
        const gltfRootNode = gltf.nodes[sceneRootNodeIndex];

        const scene = new Scene();
        scene.name = Object.hasOwn(gltfRootNode, "name") ? gltfRootNode.name : `scene_${sceneRootNodeIndex}`;
        scene.materialBundle = new MaterialBundle(this.gltfData);

        this._parseSceneExtras(scene, gltfRootNode);
        this._parseCollisionMeshes(scene, gltfRootNode);
        const gltfSceneNodeRoot = this._findChildNodeStartingWith(gltfRootNode, "Scene");
        if (!gltfSceneNodeRoot) {
            return null;
        }

        this._parseNode(scene, null, gltfSceneNodeRoot);

        // Assign Node Children pointers
        for (const node of scene.nodes) {
            if (node.childNodes.length === 0) {
                continue;
            }

            let currentChild = node.childNodes[0];
            node.firstChildNode = currentChild;

            for (let i = 1; i < node.childNodes.length; i++) {
                currentChild.nextSiblingNode = node.childNodes[i];
                currentChild = node.childNodes[i];
            }
        }

        return scene;
    }

    _parseNodeExtras(scene, gltfNode, node) {
        if (!Object.hasOwn(gltfNode, "extras")) {
            return;
        }

        const extras = gltfNode.extras;

        if (Object.hasOwn(extras, "layers")) {
            const layerNames = extras.layers.split(' ');
            let layerMask = 0;

            for (const layerName of layerNames) {
                if (!this.layerMap.has(layerName))
                    throw new Error(`Node ${node.name}: No layer map value found for layer: ${layerName}`);

                layerMask |= this.layerMap.get(layerName);
            }

            node.layerMask = layerMask;
        }

        if (Object.hasOwn(extras, "data")) {
            const dataValue = parseInt(extras.data);
            
            if (isNaN(dataValue)) {
                throw new Error(`${node.name}: Unable to parse int value for data '${extras.data}'`);
            }

            node.data = dataValue;
        }

        if (Object.hasOwn(extras, "mesh")) {
            scene.meshInstanceCount += 1;
            // if the mesh is ignored just set an empty collider
            if (extras.mesh === "ignore"){
                node.mesh = N64Node.MeshIgnored;
                node.collider = N64Node.ColliderType.None
            }
        }

        if (Object.hasOwn(extras, "collider")) {
            const colliderName = extras.collider;

            if (colliderName === "none") {
                node.collider = N64Node.NoCollider;
            }
            else {
                if (!this.collisonMeshMap.has(colliderName)) {
                    throw new Error(`Node ${node.name}: invalid collion mesh name specified: ${colliderName}`);
                }

                const collisionMeshIndex = this.collisonMeshMap.get(colliderName);
                node.collider = N64Node.ColliderType.CollisionMesh | (collisionMeshIndex << 16);
            }
        }
    }

    _reset() {
        this.gltfData = new GLTFLoader();
        this.scenes = [];
        this.layerMap = null;
    }

    _extractNodeTransform(gltfNode, node) {
        if (gltfNode.translation) {
            node.position = gltfNode.translation.slice();
        }

        if (gltfNode.rotation) {
            node.rotation = gltfNode.rotation.slice();
        }

        if (gltfNode.scale) {
            node.scale = gltfNode.scale.slice();
        }

        // Note: node's parent should always be parsed before any child.
        node.computeWorldMatrix();
    }

    /** Precondition: node has its trasnform values set*/
    _getCustomBoundingBoxForNode(scene, gltfBoundingNode, node) {
        const center = Object.hasOwn(gltfBoundingNode, "translation") ? gltfBoundingNode.translation : [0.0, 0.0, 0.0];
        const extents = Object.hasOwn(gltfBoundingNode, "scale") ? gltfBoundingNode.scale : [1.0, 1.0, 1.0];
        
        const bounding = new Bounding();
        bounding.setFromCenterAndExtents(center, extents);

        const boundingBoxIndex = scene.customBoundingBoxes.length;
        scene.customBoundingBoxes.push(bounding);
        node.collider = N64Node.ColliderType.Box | (boundingBoxIndex << 16)
    }

    _parseNodeMesh(scene, gltfNode, node) {
        if (node.mesh == N64Node.MeshIgnored) {
            node.mesh = N64Node.NoMesh;
            return;
        }

        if (!Object.hasOwn(gltfNode, "mesh")) {
            return;
        }

        node.mesh = scene.bundleMeshIndex(gltfNode.mesh);
        scene.meshInstanceCount += 1;

        if (node.collider === N64Node.NoCollider) {
            node.collider = N64Node.ColliderType.Box | (GLTFLoader.BoxColliderUseMeshBounding);
        }
    }
}

module.exports = LevelParser;