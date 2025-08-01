const CollisionGeometry = require("./CollisionGeometry");
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

    createCollisionGeometry() {
        for (const scene of this.scenes) {
            scene.collisionGeometry = CollisionGeometry.createFromScene(scene, this.gltfData);
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

    _parseSceneCollisionGeometryConfig(scene, sceneGltfNode){
        const hasCollisionGeometryConfig = Object.hasOwn(sceneGltfNode, "extras") && Object.hasOwn(sceneGltfNode.extras, "gridSize");
        if (!hasCollisionGeometryConfig) {
            return;
        }

        const dimensions = sceneGltfNode.extras.gridSize.split("x");
        if (dimensions.length !== 2) {
            throw new Error(`${sceneGltfNode.name}: Unable to determine parse grid dimensions: ${sceneGltfNode.extras.gridSize.split}`);
        }

        scene.gridSize = [parseInt(dimensions[0]), parseInt(dimensions[1])];
    }

    _parseSceneExtras(scene, sceneGltfNode) {
        // TODO: Add more supported Extra fields

        if (!Object.hasOwn(sceneGltfNode, "extras")) {
            return;
        }

        const extras = sceneGltfNode["extras"];

        if (Object.hasOwn(extras, "extraColliders")) {
            const extraColliders = parseInt(extras["extraColliders"]);

            if (isNaN(extraColliders)) {
                throw new Error(`Unable to parse extraColliders value for scene: ${scene.name}`);
            }

            scene.colliderCount += extraColliders;
        }

        if (Object.hasOwn(extras, "extraMeshInstances")) {
            const extraMeshInstances = parseInt(extras["extraMeshInstances"]);

            if (isNaN(extraMeshInstances)) {
                throw new Error(`Unable to parse extraMeshInstances value for scene: ${scene.name}`);
            }

            scene.meshInstanceCount += extraMeshInstances;
        }

        if (Object.hasOwn(extras, "extraSkinnedMeshInstances")) {
            const extraSkinnedMeshInstances = parseInt(extras["extraSkinnedMeshInstances"]);

            if (isNaN(extraSkinnedMeshInstances)) {
                throw new Error(`Unable to parse extraSkinnedMeshInstances value for scene: ${scene.name}`);
            }

            scene.skinnedMeshInstanceCount += extraSkinnedMeshInstances;
        }

        if (Object.hasOwn(extras, "extraMeshes")) {
            const extraMeshes = parseInt(extras["extraMeshes"]);

            if (isNaN(extraMeshes)) {
                throw new Error(`Unable to parse extraMeshes value for scene: ${scene.name}`);
            }

            scene.extraMeshCount += extraMeshes;
        }

        if (Object.hasOwn(extras, "extraNodes")) {
            const extraNodes = parseInt(extras["extraNodes"]);

            if (isNaN(extraNodes)) {
                throw new Error(`Unable to parse extraNodes value for scene: ${scene.name}`);
            }

            scene.extraNodeCount += extraNodes;
        }
    }

    _parseNode(scene, parentNode, gltfNode) {
        const node = LevelParser.createAndAddNode(scene, parentNode);
        node.name = parentNode === null ? "root" : gltfNode.name; 

        const gltf = this.gltfData.gltf;

        this._extractNodeTransform(gltfNode, node);
        this._parseNodeExtras(scene, gltfNode, node);
        this._parseNodeMesh(scene, gltfNode, node);

        if (Object.hasOwn(gltfNode, "children")) {
            for (const childIndex of gltfNode.children) {
                const gltfChildNode = gltf.nodes[childIndex];
                const nodeHasName = Object.hasOwn(gltfChildNode, "name");

                // Note: currently custom bounding volumes are set using child nodes beginning with _boundingXXX.
                // We do not want to parse that node in this case
                if (nodeHasName) {
                    if (gltfChildNode.name.startsWith("_boundingBox")) {
                        this._getCustomBoundingBoxForNode(scene, gltfChildNode, node);
                        continue;
                    } else if (gltfChildNode.name.startsWith("_boundingSphere")) {
                        this._getCustomBoundingSphereForNode(scene, gltfChildNode, node);
                        continue
                    }
                }

                this._parseNode(scene, node, gltfChildNode);
            }
        }

        if (node.collider === N64Node.UnspecifiedCollider) {
            node.collider = N64Node.NoCollider
        }

        if (node.collider != N64Node.NoCollider) {
            scene.colliderCount += 1;
        }
    }

    static createAndAddNode(scene, parentNode) {
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
        this._parseSceneCollisionGeometryConfig(scene, gltfRootNode);
        this._parseCollisionMeshes(scene, gltfRootNode);
        const gltfSceneNodeRoot = this._findChildNodeStartingWith(gltfRootNode, "Scene");
        if (!gltfSceneNodeRoot) {
            throw new Error(`Invalid Scene structure detected for root node: ${gltfRootNode.name}`);
        }

        this._parseNode(scene, null, gltfSceneNodeRoot);

        LevelParser.assignNodeChildPointers(scene);

        for (const meshIndex of scene.meshBundle) {
            this.gltfData.validateMeshPrimitives(this.gltfData.meshes[meshIndex]);
        }

        return scene;
    }

    /** Parent is assigned via AddOrCreate, however the children pointers need
     *  to be setup before writing.
     */
    static assignNodeChildPointers(scene) {
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
            // Node extras may specify that the mesh attached to the node should be ignored.
            // This is useful for creating stand in geometry in blender that will be replaced
            // by something else at runtime.
            if (extras.mesh === "ignore" || extras.mesh === "none"){
                node.mesh = N64Node.MeshIgnored;
            }
        }

        if (Object.hasOwn(extras, "collider")) {
            const colliderName = extras.collider;

            if (colliderName === "none") {
                node.collider = N64Node.NoCollider;
            }
            else {
                if (!this.collisonMeshMap.has(colliderName)) {
                    throw new Error(`Node ${node.name}: invalid collision mesh name specified: ${colliderName}`);
                }

                const collisionMeshIndex = this.collisonMeshMap.get(colliderName);
                node.collider = N64Node.ColliderType.CollisionMesh | (collisionMeshIndex << 16);
            }
        }

        if (Object.hasOwn(extras, "collisionType")) {
            if (extras.collisionType.toLowerCase() === "dynamic") {
                node.collisionType = N64Node.CollisionType.Dynamic;
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

    /** bounding spheres will use the same storage as boxes just to keep things simpler, result is two extra non used floats */
    _getCustomBoundingSphereForNode(scene, gltfBoundingNode, node) {
        const center = Object.hasOwn(gltfBoundingNode, "translation") ? gltfBoundingNode.translation : [0.0, 0.0, 0.0];
        const scale = Object.hasOwn(gltfBoundingNode, "scale") ? gltfBoundingNode.scale : [1.0, 1.0, 1.0];

        const radius = Math.max(scale[0], scale[1], scale[2]);
        const boundingBoxIndex = scene.customBoundingBoxes.length;
        scene.customBoundingBoxes.push(Bounding.createFromMinMax(center, [radius, radius, radius]));
        node.collider = N64Node.ColliderType.Sphere | (boundingBoxIndex << 16);
    }

    _parseNodeMesh(scene, gltfNode, node) {
        // This was set when parsing node extras
        if (node.mesh == N64Node.MeshIgnored || !Object.hasOwn(gltfNode, "mesh")) {
            node.mesh = N64Node.NoMesh;
            return;
        }

        node.mesh = scene.bundleMeshIndex(gltfNode.mesh);
        scene.meshInstanceCount += 1;

        // TODO: do we actually want to do this? this should maybe be done explicitly?
        if (node.collider === N64Node.UnspecifiedCollider) {
            node.collider = N64Node.ColliderType.Box | (GLTFLoader.BoxColliderUseMeshBounding);
        }
    }
}

module.exports = LevelParser;