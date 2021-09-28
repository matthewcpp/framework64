const N64Image = require("./N64Image");
const N64Material = require("./N64Material");
const N64Mesh = require("./N64Mesh");
const N64MeshResources = require("./N64MeshResources");
const N64Primitive = require("./N64Primitive");
const N64Texture = require("./N64Texture");

const glMatrix = require("gl-matrix");

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
    mesh = null;
    resources = null;

    // these maps provide a way to convert from the GLTF file level indices to the currently loaded mesh's indices
    imageMap = new Map();
    textureMap = new Map();
    materialMap = new Map();


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

    _loadFile(gltfPath) {
        this.gltfPath = gltfPath;
        this.gltf = JSON.parse(fs.readFileSync(gltfPath, {encoding: "utf8"}));

        this.resources = new N64MeshResources();
        this.loadedBuffers.clear();
        this.imageMap.clear();
        this.textureMap.clear();
        this.materialMap.clear();
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
                this._readTexCoords(gltfPrimitive, primitive);

            this._readIndices(gltfPrimitive, primitive);
        }
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
        const imagePath = path.join(gltfDir, gltfImage.uri);


        //TODO: This should probably be more robust: e.g. image.something.ext will break asset macro name
        const imageName = path.basename(gltfImage.uri, path.extname(gltfImage.uri));
        const image = new N64Image(imageName);
        await image.load(imagePath);

        if (this.options.resizeImages.hasOwnProperty(gltfImage.uri)) {
            const dimensions = this.options.resizeImages[gltfImage.uri].split("x");
            image.data.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
            console.log(`Resize image: ${gltfImage.uri} to ${dimensions[0]}x${dimensions[1]}`);
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
        this.resources.textures.push(new N64Texture(sourceImage));

        // TODO: Get sampler info (repeat, etc)

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
                parseInt(position[0]), parseInt(position[1]), parseInt(position[2]),
                0, /* unused flag*/
                0, 0, /* texcords */
                0, 0, 0, 0 /* color or normal */
            ];

            primitive.vertices.push(vertex);
            primitive.bounding.encapsulatePoint(position);
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
                vertex[6] = Math.round(buffer.readUInt16LE(offset) / 0xFF);
                vertex[7] = Math.round(buffer.readUInt16LE(offset + 2) / 0xFF);
                vertex[8] = Math.round(buffer.readUInt16LE(offset + 4) / 0xFF);
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

    _readTexCoords(gltfPrimitive, primitive) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.TEXCOORD_0];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const material = this.resources.materials[this.materialMap.get(gltfPrimitive.material)];

        if (material.texture === N64Material.NoTexture) {
            console.log("No image specified.  Ignoring texture coordinates.")
            return;
        }

        const texture = this.resources.textures[material.texture];
        const image = this.resources.images[texture.image];

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);

        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = primitive.vertices[i];

            let s = buffer.readFloatLE(offset);
            let t = buffer.readFloatLE(offset + 4);

            // TODO: this needs to be fixed, its not correct
            // clamp tex coords to (0, 1)
            s = Math.min(Math.max(s, 0.0), 1.0) * image.width * 2;
            t = Math.min(Math.max(t, 0.0), 1.0) * image.height * 2;

            // Note that the texture coordinates (s,t) are encoded in S10.5 format.
            vertex[4] = Math.round(s * (1 << 5));
            vertex[5] = Math.round(t * (1 << 5));

            offset += byteStride;
        }
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