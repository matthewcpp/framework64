const N64Image = require("./N64Image");
const N64Material = require("./N64Material");
const N64Mesh = require("./N64Mesh");
const N64Node = require("./N64Node");
const N64MeshResources = require("./N64MeshResources");
const N64Primitive = require("./N64Primitive");
const N64Texture = require("./N64Texture");
const N64Scene = require("./N64Scene")
const Util = require("../Util")

const glMatrix = require("gl-matrix");

const fs = require("fs");
const path = require("path");
const Bounding = require("./Bounding");

const GltfComponentType = {
    Byte: 5120,
    UnsignedByte: 5121,
    Short: 5122,
    UnsignedShort: 5123,
    UnsignedInt: 5125,
    Float: 5126
}

class GLTFLoader {
    loadedBuffers = new Map();
    gltfPath = null;
    gltf = null;
    mesh = null;
    resources = null;

    // these maps provide a way to convert from the GLTF file level indices to the currently loaded mesh's indices
    imageMap = new Map();
    textureMap = new Map();
    materialMap = new Map();
    meshMap = new Map();
    collisionMeshMap = new Map();

    static InvalidNodeIndex = -1;

    /** Indicates that a node should use its attached mesh for bounding*/
    static BoxColliderUseMeshBounding = (256 << 16);
    /** this holds the index into the nodes array of the root scene node. Not to be confused with the GLTF scene node index. */
    _sceneNodeIndex = GLTFLoader.InvalidNodeIndex;

    /** This holds the index of the current mesh collider node that is being parsed. i.e. scene node / Colliders / <this node> */
    _meshColliderNodeIndex = GLTFLoader.InvalidNodeIndex;

    constructor(options) {
        this.options =  {
            resizeImages: {}
        };

        Object.assign(this.options, options);
    }

    /** Loads the first mesh found in the 'meshes' array of a GLTF File. */
    async loadStaticMesh(gltfPath) {
        this._loadFile(gltfPath);

        if (!this.gltf.meshes || this.gltf.meshes.length === 0) {
            throw new Error(`GLTF File: ${gltfPath} contains no meshes`);
        }

        const modelName = path.basename(gltfPath, ".gltf");
        this.mesh = new N64Mesh(modelName);
        this.mesh.resources = this.resources;
        await this._loadMesh(this.gltf.meshes[0]);

        return this.mesh;
    }

    _resetMaps() {
        this.resources = new N64MeshResources();
        this.loadedBuffers.clear();
        this.imageMap.clear();
        this.textureMap.clear();
        this.materialMap.clear();
        this.meshMap.clear();
        this.collisionMeshMap.clear();
    }

    async loadLevel(gltfPath, layerMap) {
        this._loadFile(gltfPath);

        const rootNode = this.gltf.scenes[this.gltf.scene];

        const scenes = [];

        for (const sceneIndex of rootNode.nodes) {
            const scene = await this._parseScene(sceneIndex, layerMap);
            if (scene)
                scenes.push(scene);
        }

        return scenes;
    }

    _extractNodeTransform(gltfNode, node) {
        if (gltfNode.translation)
            node.position = gltfNode.translation.slice();

        if (gltfNode.rotation)
            node.rotation = gltfNode.rotation.slice();

        if (gltfNode.scale)
            node.scale = gltfNode.scale.slice();
    }

    /** Precondition: node has its trasnform values set and gltfNode has children  */
    _getCustomBoundingBoxForNode(scene, gltfNode, node) {
        for (const childIndex of gltfNode.children) {
            const childNode = this.gltf.nodes[childIndex];

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

    async _parseScene(sceneRootNodeIndex, layerMap) {
        this._resetMaps();
        this._parseSceneNode(sceneRootNodeIndex);

        // the passed in `sceneRootNodeIndex` was not a valid scene root
        if (this._sceneNodeIndex === GLTFLoader.InvalidNodeIndex)
            return null;

        const rootNode = this.gltf.nodes[sceneRootNodeIndex];
        const sceneNode = this.gltf.nodes[this._sceneNodeIndex];

        const scene = new N64Scene();
        scene.name = Util.safeDefineName(rootNode.name.toLowerCase());
        scene.meshResources = this.resources;
        scene.rootNode = sceneNode;

        if (!sceneNode.children) // empty scene
            return scene;

        for (const child_index of sceneNode.children) {
            const gltfNode = this.gltf.nodes[child_index];
            const node = new N64Node();

            const hasMesh = gltfNode.hasOwnProperty("mesh");

            this._extractNodeTransform(gltfNode, node);

            if (gltfNode.hasOwnProperty("children")) {
                this._getCustomBoundingBoxForNode(scene, gltfNode, node);
            }

            if (hasMesh) {
                // if we already loaded this mesh then grab the index of it in the scene mesh array
                if (this.meshMap.has(gltfNode.mesh)) {
                    node.mesh = this.meshMap.get(gltfNode.mesh);
                }
                else { // load mesh we haven't seen before
                    this.mesh = new N64Mesh(gltfNode.name);
                    await this._loadMesh(this.gltf.meshes[gltfNode.mesh]);
                    node.mesh = scene.meshes.length;
                    scene.meshes.push(this.mesh);
                    this.meshMap.set(gltfNode.mesh, node.mesh);
                }

                if (node.collider === N64Node.NoCollider)
                    node.collider = N64Node.ColliderType.Box | (GLTFLoader.BoxColliderUseMeshBounding);
            }

            if (gltfNode.hasOwnProperty("extras")) {
                if (gltfNode.extras.hasOwnProperty("layers")) {
                    const layerNames = gltfNode.extras.layers.split(' ');
                    let layerMask = 0;

                    for (const layerName of layerNames) {
                        if (!layerMap.has(layerName))
                            throw new Error(`Node ${gltfNode.name}: No layer map value found for layer: ${layerName}`);

                        layerMask |= layerMap.get(layerName);
                    }

                    node.layerMask = layerMask;
                }

                if (gltfNode.extras.hasOwnProperty("collider")) {
                    const colliderName = gltfNode.extras.collider;

                    if (colliderName === "none") {
                        node.collider = N64Node.NoCollider;
                    }
                    else {
                        let meshColliderIndex;

                        if (this.collisionMeshMap.has(colliderName)) {
                            meshColliderIndex = this.collisionMeshMap.get(colliderName);
                        }
                        else {
                            meshColliderIndex = scene.collisionMeshes.length;
                            await this._parseCollisionMap(colliderName);
                            scene.collisionMeshes.push(this.mesh);
                            this.collisionMeshMap.set(colliderName, meshColliderIndex);
                        }
    
                        node.collider = N64Node.ColliderType.Mesh | (meshColliderIndex << 16);
                    }
                }
            }

            if (node.collider != N64Node.ColliderType.None) {
                scene.colliderCount += 1;
            }

            scene.nodes.push(node);
        }

        return scene;
    }

    async _parseCollisionMap(name) {
        if (this._meshColliderNodeIndex === -1)
            throw new Error("No collision mesh root node present in scene.");

        const collisionMeshParentNode = this.gltf.nodes[this._meshColliderNodeIndex];
        let collisionMeshIndex = -1;

        for (const child of collisionMeshParentNode.children) {
            const node = this.gltf.nodes[child];

            if (node.name !== name)
                continue;

            if (node.hasOwnProperty("mesh"))
                collisionMeshIndex = node.mesh;
        }

        if (collisionMeshIndex === -1)
            throw new Error(`Unable to locate collision mesh with name: ${name}`)

        this.mesh = new N64Mesh(name);
        await this._loadMesh(this.gltf.meshes[collisionMeshIndex]);
    }

    /** Parses the scene node from the glTF JSON to determine the root nodes for the scene and mesh colliders. */
    _parseSceneNode(sceneIndex) {
        this._sceneNodeIndex = GLTFLoader.InvalidNodeIndex;
        this._meshColliderNodeIndex = GLTFLoader.InvalidNodeIndex;

        const sceneNode = this.gltf.nodes[sceneIndex];

        if (!sceneNode.children)
            return;

        for (const nodeIndex of sceneNode.children) {
            const node = this.gltf.nodes[nodeIndex];

            if (!node.hasOwnProperty("name"))
                continue;

            if (node.name.startsWith("Scene")) {
                this._sceneNodeIndex = nodeIndex;
            }

            if (node.name.startsWith("Colliders")) {
                this._meshColliderNodeIndex = nodeIndex;
            }
        }
    }

    _loadFile(gltfPath) {
        this.gltfPath = gltfPath;
        this.gltf = JSON.parse(fs.readFileSync(gltfPath, {encoding: "utf8"}));

        this._resetMaps();
    }

    async _loadMesh(gltfMesh) {
        const supportedPrimitiveModes = new Set(Object.values(N64Primitive.ElementType));

        for (const gltfPrimitive of gltfMesh.primitives) {
            // only support triangles for now
            const mode = gltfPrimitive.hasOwnProperty("mode") ? gltfPrimitive.mode : N64Primitive.ElementType.Triangles;

            if (!supportedPrimitiveModes.has(mode)) {
                console.log(`Skipping primitive with mode: ${mode}`);
                continue;
            }

            const primitive = new N64Primitive(mode);
            this.mesh.primitives.push(primitive);

            primitive.material = await this._getMaterial(gltfPrimitive.hasOwnProperty("material") ? gltfPrimitive.material : -1);

            this._readPositions(gltfPrimitive, primitive);

            if (gltfPrimitive.attributes.COLOR_0) {
                this._readVertexColors(gltfPrimitive, primitive);
            }
            else if (gltfPrimitive.attributes.NORMAL) {
                this._readNormals(gltfPrimitive, primitive);
            }

            if (gltfPrimitive.attributes.TEXCOORD_0)
                this._readTexCoords(gltfPrimitive, primitive, gltfMesh.name);

            if (gltfPrimitive.attributes.JOINTS_0)
                this._readJointIndices(gltfPrimitive, primitive);

            this._readIndices(gltfPrimitive, primitive);

            const material = this.resources.materials[primitive.material];
            if (material.shadingMode === N64Material.ShadingMode.Unset) {
                material.shadingMode = this._determineShadingMode(primitive, material);
            }
        }

        this.mesh.prunePrimitiveVertices();
    }

    _determineShadingMode(primitive, material) {
        if (primitive.hasVertexColors) {
            if (material.hasTexture)
                return N64Material.ShadingMode.VertexColorsTextured;
            else
                return N64Material.ShadingMode.VertexColors;
        }

        if (primitive.hasNormals) {
            if (material.hasTexture)
                return N64Material.ShadingMode.GouraudTextured;
            else
                return N64Material.ShadingMode.Gouraud;
        }

        throw new Error("Could not determine shading mode for primitive");
    }

    /** Gets the correct index for the GLTF material.  I
     * If a negative index is passed in, a default material will be used. **/
    async _getMaterial(index) {
        if (index < 0) {
            const materialIndex = this.resources.materials.length;
            this.resources.materials.push(new N64Material());
            return materialIndex;
        }

        // we have already parsed this material
        if (this.materialMap.has(index)) {
            return this.materialMap.get(index);
        }

        // new material
        const materialIndex = this.resources.materials.length;
        const material = new N64Material();

        const gltfMaterial = this.gltf.materials[index];
        const pbr = gltfMaterial.pbrMetallicRoughness;

        if (pbr.baseColorFactor) {
            const baseColor = pbr.baseColorFactor;
            material.color[0] = Math.round(baseColor[0] * 255);
            material.color[1] = Math.round(baseColor[1] * 255);
            material.color[2] = Math.round(baseColor[2] * 255);
            material.color[3] = Math.round(baseColor[3] * 255);
        }

        if (pbr.baseColorTexture) {
            material.texture = await this._getTexture(pbr.baseColorTexture.index);
        }

        // Applies a specific shading mode to this material.  In this case mode will not be determined later.
        if (gltfMaterial.extras && gltfMaterial.extras.shadingMode) {
            if (N64Material.ShadingMode.hasOwnProperty(gltfMaterial.extras.shadingMode)) {
                material.shadingMode = N64Material.ShadingMode[gltfMaterial.extras.shadingMode];
            }
            else {
                throw new Error(`Unsupported Shading Mode specified in Material: ${gltfMaterial.name}: ${gltfMaterial.extras.shadingMode}`);
            }
        }

        this.resources.materials.push(material)
        this.materialMap.set(index, materialIndex);

        return materialIndex;
    }

    async _getImage(gltfIndex) {
        if (this.imageMap.has(gltfIndex)) {
            return this.imageMap.get(gltfIndex);
        }

        const imageIndex = this.resources.images.length;
        const gltfDir = path.dirname(this.gltfPath);

        const gltfImage = this.gltf.images[gltfIndex];
        const imageURI = decodeURI(gltfImage.uri);
        const imagePath = path.join(gltfDir, imageURI);


        //TODO: This should probably be more robust: e.g. image.something.ext will break asset macro name
        const imageName = Util.safeDefineName(path.basename(imageURI, path.extname(imageURI)));
        const image = new N64Image(imageName, N64Image.Format.RGBA16);
        await image.load(imagePath);

        if (this.options.resizeImages.hasOwnProperty(imageURI)) {
            const dimensions = this.options.resizeImages[imageURI].split("x");
            image.data.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
            console.log(`Resize image: ${imageURI} to ${dimensions[0]}x${dimensions[1]}`);
        }

        this.resources.images.push(image);
        this.imageMap.set(gltfIndex, imageIndex);

        return imageIndex;
    }

    async _getTexture(gltfIndex) {
        if (this.textureMap.has(gltfIndex)) {
            return this.textureMap.get(gltfIndex);
        }

        const textureIndex = this.resources.textures.length;
        const gltfTexture = this.gltf.textures[gltfIndex];

        const sourceImage = await this._getImage(gltfTexture.source);
        const texture = new N64Texture(sourceImage);
        this.resources.textures.push(texture);

        // TODO: Get sampler info (repeat, etc)
        const image = this.resources.images[sourceImage];
        texture.maskS = Math.log2(image.width);
        texture.maskT = Math.log2(image.height);

        this.textureMap.set(gltfIndex, textureIndex);
        return textureIndex;
    }

    _getBuffer(bufferIndex) {
        let buffer = this.loadedBuffers.get(bufferIndex);

        if (!buffer) {
            const gltfBuffer = this.gltf.buffers[bufferIndex];

            const bufferDir = path.dirname(this.gltfPath);
            const bufferPath = path.join(bufferDir, gltfBuffer.uri);

            buffer = fs.readFileSync(bufferPath);
            this.loadedBuffers.set(bufferIndex, buffer);
        }

        return buffer;
    }

    _readPositions(gltfPrimitive, primitive) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.POSITION];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        // by default position vec3's are tightly packed
        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : 12;

        let offset = bufferView.byteOffset;

        for (let i = 0; i < accessor.count; i++) {
            const position = [
                buffer.readFloatLE(offset),
                buffer.readFloatLE(offset + 4),
                buffer.readFloatLE(offset + 8)
            ];

            const vertex = [
                Math.round(position[0]), Math.round(position[1]), Math.round(position[2]),
                0, /* unused flag*/
                0, 0, /* texcords */
                0, 0, 0, 0 /* color or normal */
            ];

            primitive.vertices.push(vertex);
            primitive.bounding.encapsulatePoint(position);
            offset += byteStride;
        }
    }

    _readJointIndices(gltfPrimitive, primitive) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.JOINTS_0];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        if (accessor.componentType !== GltfComponentType.UnsignedByte) {
            throw new Error("Currently only vertex weights specified as Unsigned byte are supported.");
        }

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : 4;
        let offset = bufferView.byteOffset;

        primitive.jointIndices = [];

        // TODO: Validate that there are no other non zero weighted indices
        for (let i = 0; i < accessor.count; i++) {
            primitive.jointIndices.push(buffer.readUInt8(offset));

            offset += byteStride;
        }
    }

    _getDefaultStride(type, componentType) {
        let componentCount = 0;
        let componentSize = 0;

        switch (componentType) {
            case GltfComponentType.Byte:
            case GltfComponentType.UnsignedByte:
                componentSize = 1;
                break;

            case GltfComponentType.Short:
            case GltfComponentType.UnsignedShort:
                componentSize = 2;
                break;

            case GltfComponentType.UnsignedInt: /* unsigned int */
            case GltfComponentType.Float: /* float */
                componentSize = 4;
                break;

            default:
                throw new Error(`Cannot determine component size for: ${componentType}`);
        }

        switch(type) {
            case "SCALAR":
                componentCount = 1;
                break;

            case "VEC2":
                componentCount = 2
                break;

            case "VEC3":
                componentCount = 3;
                break;

            case "VEC4":
                componentCount = 4;
                break;

            default:
                throw new Error(`Cannot determine component count for: ${type}`);
        }

        return componentSize * componentCount;
    }

    _readVertexColors(gltfPrimitive, primitive) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.COLOR_0];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);
        let offset = bufferView.byteOffset;

        let parseVertexColor = null;
        if (accessor.componentType === GltfComponentType.Float){
            parseVertexColor = (vertex, buffer, offset) => {
                vertex[6] = Math.round(buffer.readFloatLE(offset) * 255);
                vertex[7] = Math.round(buffer.readFloatLE(offset + 4) * 255);
                vertex[8] = Math.round(buffer.readFloatLE(offset + 8) * 255);
                vertex[9] = 255;
            };
        }
        else if (accessor.componentType === GltfComponentType.UnsignedShort) { // these values need to be normalized to 0 - 255
            parseVertexColor = (vertex, buffer, offset) => {
                let r = buffer.readUInt16LE(offset);
                let g = buffer.readUInt16LE(offset + 2);
                let b = buffer.readUInt16LE(offset + 4);
                vertex[6] = Math.round( (r / 65535.0) * 255);
                vertex[7] = Math.round( (g / 65535.0) * 255);
                vertex[8] = Math.round( (b / 65535.0) * 255);
                vertex[9] = 255;
            };
        }
        else {
            throw new Error(`Unsupported Vertex color type: ${accessor.componentType}`);
        }

        for (let i = 0; i < accessor.count; i++) {
            parseVertexColor(primitive.vertices[i], buffer, offset);
            offset += byteStride;
        }

        primitive.hasVertexColors = true;
    }

    _readNormals(gltfPrimitive, primitive) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.NORMAL];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);

        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = primitive.vertices[i];

            // the normals are treated as 8-bit signed values (-128 to 127).
            vertex[6] = Math.min(Math.round(buffer.readFloatLE(offset) * 128), 127);
            vertex[7] = Math.min(Math.round(buffer.readFloatLE(offset + 4) * 128), 127);
            vertex[8] = Math.min(Math.round(buffer.readFloatLE(offset + 8) * 128), 127);
            vertex[9] = 255

            offset += byteStride;
        }

        primitive.hasNormals = true;
    }

    // ensure that the tex coord value will sit in a signed 16 bit integer
    static _validateTexCoord(val) {
        return val >= -32768 && val <= 32767;
    }

    _readTexCoords(gltfPrimitive, primitive, meshName) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.TEXCOORD_0];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const material = this.resources.materials[this.materialMap.get(gltfPrimitive.material)];

        if (material.texture === N64Material.NoTexture) {
            return;
        }

        const texture = this.resources.textures[material.texture];
        const image = this.resources.images[texture.image];

        if (!Util.isPowerOf2(image.width) || !Util.isPowerOf2(image.height)) {
            throw new Error(`image: ${image.name} has non power of 2 dimensions: ${image.width}x${image.height}`);
        }

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);

        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = primitive.vertices[i];

            let s = buffer.readFloatLE(offset);
            let t = buffer.readFloatLE(offset + 4);

            s *= image.width * 2;
            t *= image.height * 2;

            // Note that the texture coordinates (s,t) are encoded in S10.5 format.
            vertex[4] = Math.round(s * (1 << 5));
            vertex[5] = Math.round(t * (1 << 5));

            if (!GLTFLoader._validateTexCoord(vertex[4]) || !GLTFLoader._validateTexCoord(vertex[5])) {
                throw new Error(`Mesh ${meshName}: Detected a tex coord value outside of S10.5 format range.  Check model source data.`);
            }

            offset += byteStride;
        }

        primitive.hasTexCoords = true;
    }

    static _readElementList(buffer, byteOffset, count, size, componentType, mesh) {
        let offset = byteOffset;
        let element = [];

        for (let i = 0; i < count; i++) {
            if (i > 0 && (i % size === 0)) {
                mesh.elements.push(element);
                element = [];
            }

            switch (componentType) {
                case GltfComponentType.UnsignedShort:
                    element.push(buffer.readUInt16LE(offset));
                    offset += 2;
                    break;

                case GltfComponentType.UnsignedInt:
                    element.push(buffer.readUInt32LE(offset));
                    offset += 4;
                    break;

                default:
                    throw new Error(`Unsupported element type: ${componentType}`);
            }
        }

        mesh.elements.push(element);
    }

    _readIndices(gltfPrimitive, primitive) {
        const accessor = this.gltf.accessors[gltfPrimitive.indices];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const elementSize = primitive.elementType === N64Primitive.ElementType.Triangles ? 3 : 2;

        GLTFLoader._readElementList(buffer, bufferView.byteOffset, accessor.count, elementSize, accessor.componentType, primitive);
    }

}

module.exports = GLTFLoader;