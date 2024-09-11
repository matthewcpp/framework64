const GLTFVertexIndex = require("./GLTFVertexIndex")
const Material = require("./Material");
const Mesh = require("./Mesh");
const Primitive = require("./Primitive");
const Texture = require("./Texture");

const Jimp = require("jimp");

const fs = require("fs");
const path = require("path");

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

    /** This holds the _parsed_ JSON data from the accompanying images.json file */
    imagesJson = null;

    images = [];
    textures = [];
    materials = [];
    meshes = [];

    static SupportedPrimitiveModes = new Set(Object.values(Primitive.ElementType));

    static InvalidIndex = -1;
    defaultMaterialIndex = GLTFLoader.InvalidIndex;

    static InvalidNodeIndex = -1;

    /** Indicates that a node should use its attached mesh for bounding*/
    static BoxColliderUseMeshBounding = (256 << 16);
    /** this holds the index into the nodes array of the root scene node. Not to be confused with the GLTF scene node index. */
    _sceneNodeIndex = GLTFLoader.InvalidNodeIndex;

    /** This holds the index of the current mesh collider node that is being parsed. i.e. scene node / Colliders / <this node> */
    _meshColliderNodeIndex = GLTFLoader.InvalidNodeIndex;

    async loadFile(gltfPath) {
        this.gltfPath = gltfPath;
        this.gltf = JSON.parse(fs.readFileSync(gltfPath, {encoding: "utf8"}));

        this._reset();

        const imageJsonPath = path.join(path.dirname(gltfPath), "images.json");
        if (fs.existsSync(imageJsonPath))
            this.imagesJson = JSON.parse(fs.readFileSync(imageJsonPath, {encoding: "utf8"}));
        else
            this.imagesJson = [];

        await this._parseImages();
        this._parseTextures();
        this._parseMaterials();
        this._parseMeshes();
    }

    _reset() {
        if (this.gltfPath === null)
            return;

        this.images = [];
        this.textures = [];
        this.materials = [];
        this.meshes = [];
    }

    async _parseImages() {
        if (!this.gltf.images)
            return;

        for (const gltfImage of this.gltf.images) {
            const imageIndex = this.images.length;
            const imageURI = decodeURI(gltfImage.uri);

            // check if there is a specific configuration for this image
            let imageInfo = this.imagesJson.find(i => i.src === imageURI);
            if (!imageInfo) {
                imageInfo = {
                    src: imageURI,
                    hslices: 1,
                    vslices: 1,
                    format: "RGBA16"
                }
            }


            // TODO: Eventually when the Image class is standardized across platforms we want to just load it here and save it off
            // right now this results in reading each image twice.
            const imagePath = path.join(path.dirname(this.gltfPath), imageURI);
            const bitmap = await Jimp.read(imagePath);
            imageInfo.hasAlpha = bitmap.hasAlpha();

            this.images.push(imageInfo);
        }
    }

    _parseTextures() {
        if (!this.gltf.textures)
            return;

        for (const gltfTexture of this.gltf.textures) {
            const texture = new Texture(gltfTexture.source);
            // TODO: Get sampler info (repeat, etc)

            this.textures.push(texture);
        }
    }

    _parseMaterials() {
        if (!this.gltf.materials)
            return;

        for (const gltfMaterial of this.gltf.materials) {
            const material = new Material();

            const pbr = gltfMaterial.pbrMetallicRoughness;

            if (pbr.baseColorFactor) {
                material.setColorFromFloatArray(pbr.baseColorFactor);
            }

            if (pbr.baseColorTexture) {
                material.texture = pbr.baseColorTexture.index;
            }

            // Applies a specific shading mode to this material.  In this case mode will not be determined later.
            if (gltfMaterial.extras && gltfMaterial.extras.shadingMode) {
                const extras = gltfMaterial.extras;

                if (extras.shadingMode) {
                    if (Material.ShadingMode.hasOwnProperty(gltfMaterial.extras.shadingMode)) {
                        material.shadingMode = Material.ShadingMode[gltfMaterial.extras.shadingMode];
                    }
                    else {
                        throw new Error(`Unsupported Shading Mode specified in Material: ${gltfMaterial.name}: ${gltfMaterial.extras.shadingMode}`);
                    }
                }

                if (extras.baseColorFactor) {
                    material.setColorFromFloatArray(extras.baseColorFactor);
                }
            }

            this.materials.push(material);
        }
    }

    _parseMeshes() {
        if (!this.gltf.meshes)
            return;

        for (const gltfMesh of this.gltf.meshes) {
            const mesh = new Mesh(gltfMesh.name);

            for (const gltfPrimitive of gltfMesh.primitives) {

                const mode = ("mode" in gltfPrimitive) ? gltfPrimitive.mode : Primitive.ElementType.Triangles;
    
                if (!GLTFLoader.SupportedPrimitiveModes.has(mode)) {
                    console.log(`Skipping primitive with mode: ${mode}`);
                    continue;
                }
    
                const primitive = new Primitive(mode);
                mesh.primitives.push(primitive);
    
                primitive.material = ("material" in gltfPrimitive) ? gltfPrimitive.material : this._getDefaultMaterialIndex();
    
                this._readPositions(gltfPrimitive, primitive);
    
                if (Object.hasOwn(gltfPrimitive.attributes, "COLOR_0")) {
                    this._readVertexColors(gltfPrimitive, primitive);
                }
                else if (Object.hasOwn(gltfPrimitive.attributes, "NORMAL")) {
                    this._readNormals(gltfPrimitive, primitive);
                }
    
                if (Object.hasOwn(gltfPrimitive.attributes, "TEXCOORD_0"))
                    this._readTexCoords(gltfPrimitive, primitive, gltfMesh.name);
    
                if (Object.hasOwn(gltfPrimitive.attributes, "JOINTS_0"))
                    this._readJointIndices(gltfPrimitive, primitive);
    
                this._readIndices(gltfPrimitive, primitive);
    
                const material = this.materials[primitive.material];
                if (material.shadingMode === Material.ShadingMode.Unset) {
                    material.shadingMode = this._determineShadingMode(primitive, material);
                }
            }
    
            mesh.prunePrimitiveVertices();
            this._validateMeshPrimitives(mesh);
            mesh.sortPrimitives(this.materials);
            this.meshes.push(mesh);
        }
    }

    _validateMeshPrimitives(mesh) {
        for (let i = 0; i < mesh.primitives.length; i++) {
            const primitive = mesh.primitives[i];
            const material = this.materials[primitive.material];

            if (material.usesNormals && !primitive.hasNormals ) {
                throw new Error(`Mesh ${mesh.name} primitive ${i}: uses lit material but does not contain normals.`);
            }

            if (material.usesVertexColors && !primitive.hasVertexColors) {
                throw new Error(`Mesh ${mesh.name} primitive ${i}: uses unlit material but does not contain vertex colors.`);
            }

            if (material.hasTexture() && !primitive.hasTexCoords) {
                throw new Error(`Mesh ${mesh.name} primitive ${i}: Material uses texture but primitive does not contain tex coords.`);
            }
        }
    }

    /** Precondition: This method should only be called after all materials have been parsed. */
    _getDefaultMaterialIndex() {
        if (this.defaultMaterialIndex == GLTFLoader.InvalidIndex) {
            this.defaultMaterialIndex = this.materials.length;
            this.materials.push(new Material(this.defaultMaterialIndex));
        }

        return this.defaultMaterialIndex;
    }

    _determineShadingMode(primitive, material) {
        let materialImageHasAlpha = false;

        if (material.hasTexture()) {
            const texture = this.textures[material.texture];
            materialImageHasAlpha = this.images[texture.image].hasAlpha;
        }
        if (primitive.hasVertexColors) {
            if (material.hasAlpha || materialImageHasAlpha) {
                return Material.ShadingMode.UnlitTransparentTextured;
            } else if (material.hasTexture()) {
                return Material.ShadingMode.UnlitTextured;
            } else {
                return Material.ShadingMode.Unlit;
            }
        }

        if (primitive.hasNormals) {
            if (material.hasTexture())
                return Material.ShadingMode.LitTextured;
            else
                return Material.ShadingMode.Lit;
        }

        throw new Error("Could not determine shading mode for primitive");
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

            primitive.hasPositions = true;
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

        // TODO: does this need to be extended to handle 4 component vectors?
        let parseVertexColor = null;
        if (accessor.componentType === GltfComponentType.Float){
            parseVertexColor = (vertex, buffer, offset) => {
                vertex[GLTFVertexIndex.ColorR] = buffer.readFloatLE(offset);
                vertex[GLTFVertexIndex.ColorG] = buffer.readFloatLE(offset + 4);
                vertex[GLTFVertexIndex.ColorB] = buffer.readFloatLE(offset + 8);
                vertex[GLTFVertexIndex.ColorA] = 1.0;
            };
        }
        else if (accessor.componentType === GltfComponentType.UnsignedShort) { // these values need to be normalized to 0 - 255
            parseVertexColor = (vertex, buffer, offset) => {
                vertex[GLTFVertexIndex.ColorR] = buffer.readUInt16LE(offset) / 65535.0;
                vertex[GLTFVertexIndex.ColorG] = buffer.readUInt16LE(offset + 2) / 65535.0;
                vertex[GLTFVertexIndex.ColorB] = buffer.readUInt16LE(offset + 4) / 65535.0;
                vertex[GLTFVertexIndex.ColorA] = 1.0;
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

            vertex[6] = buffer.readFloatLE(offset);
            vertex[7] = buffer.readFloatLE(offset + 4);
            vertex[8] = buffer.readFloatLE(offset + 8);
            vertex[9] = 1.0

            offset += byteStride;
        }

        primitive.hasNormals = true;
    }

    _readTexCoords(gltfPrimitive, primitive, meshName) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.TEXCOORD_0];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);

        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = primitive.vertices[i];

            vertex[4] = buffer.readFloatLE(offset);
            vertex[5] = buffer.readFloatLE(offset + 4);

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

        const elementSize = primitive.elementType === Primitive.ElementType.Triangles ? 3 : 2;

        GLTFLoader._readElementList(buffer, bufferView.byteOffset, accessor.count, elementSize, accessor.componentType, primitive);
    }

}

module.exports = GLTFLoader;