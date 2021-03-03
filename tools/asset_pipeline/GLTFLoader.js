const N64Model = require("./N64Model")
const N64Mesh = require("./N64Mesh")
const N64Material = require("./N64Material")
const N64Image = require("./N64Image");

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
    constructor(options) {
        this.options =  {
            bakeTransform : false,
            resizeImages: {}
        };

        Object.assign(this.options, options);

        this.loadedBuffers = new Map();
        this.gltfPath = null;
        this.gltf = null;
        this.transform = glMatrix.mat4.create();
        this.model = null;
    }

    async load(gltfPath) {
        this.gltfPath = gltfPath;
        this.gltf = JSON.parse(fs.readFileSync(gltfPath, {encoding: "utf8"}));

        const modelName = path.basename(gltfPath, ".gltf");
        this.model = new N64Model(modelName);

        this._readMaterials();
        await this._readImages();

        const scene = this.gltf.scenes[this.gltf.scene];

        for (const nodeIndex of scene.nodes) {
            const gltfNode = this.gltf.nodes[nodeIndex];

            if (!gltfNode.hasOwnProperty("mesh"))
                continue;

            this._processNode(gltfNode);
        }
    }

    _processNode(gltfNode) {
        this._getTransform(gltfNode);

        const gltfMesh = this.gltf.meshes[gltfNode.mesh];
        const supportedPrimitiveModes = new Set(Object.values(N64Mesh.ElementType));

        for (const gltfPrimitive of gltfMesh.primitives) {
            // only support triangles for now
            const mode = gltfPrimitive.hasOwnProperty("mode") ? gltfPrimitive.mode : N64Mesh.ElementType.Triangles;

            if (!supportedPrimitiveModes.has(mode)) {
                console.log(`Skipping primitive with mode: ${mode}`);
                continue;
            }

            const n64Mesh = new N64Mesh(mode);
            this.model.meshes.push(n64Mesh);

            this._readPositions(gltfPrimitive, n64Mesh);

            if (gltfPrimitive.attributes.COLOR_0) {
                this._readVertexColors(gltfPrimitive, n64Mesh);
            }
            else if (gltfPrimitive.attributes.NORMAL) {
                this._readNormals(gltfPrimitive, n64Mesh);
            }

            if (gltfPrimitive.attributes.TEXCOORD_0)
                this._readTexCoords(gltfPrimitive, n64Mesh);

            if (gltfPrimitive.hasOwnProperty("material"))
                n64Mesh.material = gltfPrimitive.material;
            else
                n64Mesh.material = 0; //use default

            this._readIndices(gltfPrimitive, n64Mesh);
        }
    }

    _getTransform(gltfNode) {
        const translation = gltfNode.hasOwnProperty("translation") ? gltfNode.translation.slice() : glMatrix.vec3.fromValues(0.0, 0.0, 0.0);
        const rotation = gltfNode.hasOwnProperty("rotation") ? gltfNode.rotation.slice() : glMatrix.quat.create();
        const scale = gltfNode.hasOwnProperty("scale") ? gltfNode.scale.slice() : glMatrix.vec3.fromValues(1.0, 1.0, 1.0);

        glMatrix.mat4.fromRotationTranslationScale(this.transform, rotation, translation, scale);
    }

    _getBuffer(bufferIndex) {
        let buffer = this.loadedBuffers.get(bufferIndex);

        if (!buffer) {
            const gltfBuffer = this.gltf.buffers[bufferIndex];

            const bufferDir = path.dirname(this.gltfPath);
            const bufferPath = path.join(bufferDir, gltfBuffer.uri);

            console.log(`Load Buffer: ${bufferPath}`);
            
            buffer = fs.readFileSync(bufferPath);
            this.loadedBuffers.set(bufferIndex, buffer);
        }

        return buffer;
    }

    _readPositions(gltfPrimitive, n64Mesh) {
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

            if (this.options.bakeTransform) {
                glMatrix.vec3.transformMat4(position, position, this.transform);
            }

            const vertex = [
                parseInt(position[0]), parseInt(position[1]), parseInt(position[2]),
                0, /* unused flag*/
                0, 0, /* texcords */
                0, 0, 0, 0 /* color or normal */
            ];

            n64Mesh.vertices.push(vertex);
            n64Mesh.bounding.encapsulatePoint(position);
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

    _readVertexColors(gltfPrimitive, n64Mesh) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.COLOR_0];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        if (accessor.componentType !== 5126)
            throw new Error("Currently only support float for vertex colors");

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);
        
        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = n64Mesh.vertices[i];

            vertex[6] = Math.round(buffer.readFloatLE(offset) * 255);
            vertex[7] = Math.round(buffer.readFloatLE(offset + 4) * 255);
            vertex[8] = Math.round(buffer.readFloatLE(offset + 8) * 255);
            vertex[9] = 255;

            offset += byteStride;
        }

        n64Mesh.hasVertexColors = true;
    }

    _readNormals(gltfPrimitive, n64Mesh) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.NORMAL];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);

        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = n64Mesh.vertices[i];

            // the normals are treated as 8-bit signed values (-128 to 127).
            vertex[6] = Math.min(Math.round(buffer.readFloatLE(offset) * 128), 127);
            vertex[7] = Math.min(Math.round(buffer.readFloatLE(offset + 4) * 128), 127);
            vertex[8] = Math.min(Math.round(buffer.readFloatLE(offset + 8) * 128), 127);
            vertex[9] = 255

            offset += byteStride;
        }

        n64Mesh.hasNormals = true;
    }

    _readTexCoords(gltfPrimitive, n64Mesh) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.TEXCOORD_0];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const material = this.gltf.materials[gltfPrimitive.material];

        if (!material.pbrMetallicRoughness.baseColorTexture) {
            console.log("No image specified.  Ignoring texture coordinates.")
            return;
        }
        const image = this.model.images[material.pbrMetallicRoughness.baseColorTexture.index];

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);

        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = n64Mesh.vertices[i];

            let s = buffer.readFloatLE(offset);
            let t = buffer.readFloatLE(offset + 4);

            // clamp tex coords to (0, 1)
            s = Math.min(Math.max(s, 0.0), 1.0) * image.width * 2;
            t = Math.min(Math.max(t, 0.0), 1.0) * image.height * 2;

            // Note that the texture coordinates (s,t) are encoded in S10.5 format.
            vertex[4] = Math.round(s * (1 << 5));
            vertex[5] = Math.round(t * (1 << 5));

            offset += byteStride;
        }
    }

    _readMaterials() {
        // if the file does not specify any materials use the default.
        if (!this.gltf.hasOwnProperty("materials") || this.gltf.materials.length === 0) {
            this.model.materials.push(new N64Material());
            return;
        }

        for (const gltfMaterial of this.gltf.materials) {
            const material = new N64Material();

            const pbr = gltfMaterial.pbrMetallicRoughness;

            if (pbr.baseColorFactor) {
                const baseColor = pbr.baseColorFactor;
                material.ambient[0] = parseInt(baseColor[0] * 255);
                material.ambient[1] = parseInt(baseColor[1] * 255);
                material.ambient[2] = parseInt(baseColor[2] * 255);
            }

            if (pbr.baseColorTexture) {
                material.texture = pbr.baseColorTexture.index;
            }

            this.model.materials.push(material)
        }
    }

    async _readImages() {
        if (!this.gltf.images) return;

        const gltfDir = path.dirname(this.gltfPath);

        for (const gltfImage of this.gltf.images) {
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

            this.model.images.push(image);
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

    _readIndices(gltfPrimitive, n64Mesh) {
        const accessor = this.gltf.accessors[gltfPrimitive.indices];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const elementSize = n64Mesh.elementType === N64Mesh.ElementType.Triangles ? 3 : 2;

        GLTFLoader._readElementList(buffer, bufferView.byteOffset, accessor.count, elementSize, accessor.componentType, n64Mesh);
    }

}

module.exports = GLTFLoader;