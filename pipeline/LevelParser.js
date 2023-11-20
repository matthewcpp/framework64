const GLTFLoader = require("./n64/GLTFLoader");
const MaterialBundle = require("./MaterialBundle");
const N64Node = require("./n64/N64Node");
const N64Scene = require("./n64/N64Scene");

class LevelParser {
    gltfData = null;
    layerMap = null;
    scenes = [];
    collisonMeshMap = new Map();

    /** Index into the gltf node array that represents current scne mesh collider root*/
    _meshColliderNodeIndex = GLTFLoader.InvalidNodeIndex;

    _meshes = [];

    parse(gltfPath, layerMap) {
        this._reset();

        this.layerMap = layerMap;
        this.gltfData.loadFile(gltfPath);

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

    _parseScene(sceneRootNodeIndex) {
        const gltf = this.gltfData.gltf;
        const gltfRootNode = gltf.nodes[sceneRootNodeIndex];

        const scene = new N64Scene();
        scene.name = Object.hasOwn(gltfRootNode, "name") ? gltfRootNode.name : `scene_${sceneRootNodeIndex}`;
        scene.materialBundle = new MaterialBundle(this.gltfData);

        this._parseCollisionMeshes(scene, gltfRootNode);
        const gltfSceneNodeRoot = this._findChildNodeStartingWith(gltfRootNode, "Scene");
        if (!gltfSceneNodeRoot) {
            return null;
        }

        for (const child_index of gltfSceneNodeRoot.children) {
            const gltfNode = gltf.nodes[child_index];

            const node = new N64Node();
            
            if (Object.hasOwn(gltfNode, "name")) {
                node.name = gltfNode.name;
            }

            this._extractNodeTransform(gltfNode, node);
            this._getCustomBoundingBoxForNode(scene, gltfNode, node);
            this._parseNodeExtras(gltfNode, node);
            this._parseNodeMesh(scene, gltfNode, node);

            if (node.collider != N64Node.NoCollider) {
                scene.colliderCount += 1;
            }

            scene.nodes.push(node);
        }

        return scene;
    }

    _parseNodeExtras(gltfNode, node) {
        if (!Object.hasOwn(gltfNode, "extras")) {
            return;
        }

        const extras = gltfNode.extras;

        if (Object.hasOwn(extras, "layers")) {
            const layerNames = extras.layers.split(' ');
            let layerMask = 0;

            for (const layerName of layerNames) {
                if (!this.layerMap.has(layerName))
                    throw new Error(`Node ${gltfNode.name}: No layer map value found for layer: ${layerName}`);

                layerMask |= this.layerMap.get(layerName);
            }

            node.layerMask = layerMask;
        }

        if (Object.hasOwn(extras, "data")) {
            const dataValue = parseInt(extras.data);
            
            if (isNaN(dataValue)) {
                throw new Error(`Unable to parse data value for node: ${node.name}`);
            }

            node.data = dataValue;
        }

        if (Object.hasOwn(extras, "mesh")) {
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
                    throw new Error(`Invalid collion mesh name specified: ${colliderName}`);
                }

                const collisionMeshIndex = this.collisonMeshMap.get(colliderName);
                node.collider = N64Node.ColliderType.Mesh | (collisionMeshIndex << 16);
            }
        }
    }

    _reset() {
        this.gltfData = new GLTFLoader();
        this.scenes = [];
        this.layerMap = null;
    }

    _extractNodeTransform(gltfNode, node) {
        if (gltfNode.translation)
            node.position = gltfNode.translation.slice();

        if (gltfNode.rotation)
            node.rotation = gltfNode.rotation.slice();

        if (gltfNode.scale)
            node.scale = gltfNode.scale.slice();
    }

    /** Precondition: node has its trasnform values set*/
    _getCustomBoundingBoxForNode(scene, gltfNode, node) {
        if (!Object.hasOwn(gltfNode, "children")) {
            return;
        }

        for (const childIndex of gltfNode.children) {
            const childNode =  this.gltfData.gltf.nodes[childIndex];

            if (childNode.name.indexOf("Box") === -1)
                continue;

            const parentInvScale = node.scale.map(v => v != 0 ? 1.0 / v : 0);
            const localScale = childNode.scale.slice();

            const center = childNode.hasOwnProperty("translation") ? childNode.translation.slice() : [0.0, 0.0, 0.0];
            const extents = [0.0, 0.0, 0.0];
            for (let i = 0; i < 3; i++)
                extents[i] = (parentInvScale[i] * localScale[i]);
            
            const box = new Bounding();
            box.setFromCenterAndExtents(center, extents);

            const boundingBoxIndex = scene.customBoundingBoxes.length;
            scene.customBoundingBoxes.push(box);
            node.collider = N64Node.ColliderType.Box | (boundingBoxIndex << 16)

            break;
        }
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

        if (node.collider === N64Node.NoCollider)
            node.collider = N64Node.ColliderType.Box | (GLTFLoader.BoxColliderUseMeshBounding);
    }

    _parseCollisionMap(name) {
        if (this._meshColliderNodeIndex === -1)
            throw new Error("No collision mesh root node present in scene.");

        const collisionMeshParentNode = this.gltf.nodes[this._meshColliderNodeIndex];
        let collisionMeshIndex = -1;

        for (const child of collisionMeshParentNode.children) {
            const gltfNode = this.gltf.nodes[child];

            if (gltfNode.name !== name)
                continue;

            if (gltfNode.hasOwnProperty("mesh"))
                collisionMeshIndex = gltfNode.mesh;
        }

        if (collisionMeshIndex === -1)
            throw new Error(`Unable to locate collision mesh with name: ${name}`)

        return gltfNode;

    }
}

module.exports = LevelParser;