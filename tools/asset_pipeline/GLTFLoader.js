const N64Model = require("./N64Model")
const N64Mesh = require("./N64Mesh")
const N64Material = require("./N64Material")
const N64Image = require("./N64Image");

const glMatrix = require("gl-matrix");

const fs = require("fs");
const path = require("path");

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

        for (const gltfPrimitive of gltfMesh.primitives) {
            // only support triangles for now
            if (gltfPrimitive.hasOwnProperty("mode") && gltfPrimitive.mode != 4)
                continue;

            const n64Mesh = new N64Mesh();
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

    merge() {
        this.model.mergeVertexColorMeshes();
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
            case 5120: /* byte */
            case 5121: /* unsigned byte */
                componentSize = 1;
                break;

            case 5122: /* short */
            case 5123: /* unsigned short */
                componentSize = 2;
                break;

            case 5125: /* unsigned int */
            case 5126: /* float */
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

        if (accessor.componentType != 5126)
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
    }

    _readNormals(gltfPrimitive, n64Mesh) {
        const accessor = this.gltf.accessors[gltfPrimitive.attributes.NORMAL];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : this._getDefaultStride(accessor.type, accessor.componentType);

        let offset = bufferView.byteOffset;
        for (let i = 0; i < accessor.count; i++) {
            const vertex = n64Mesh.vertices[i];

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

            vertex[4] = Math.round(s * (1 << 5));
            vertex[5] = Math.round(t * (1 << 5));

            offset += byteStride;
        }

        n64Mesh.hasTexCoords = true;
    }

    _readMaterials() {
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

        // if the file does not specify any materials use the default.
        if (this.model.materials.length === 0) {
            this.model.materials.push(new N64Material());
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

    _readUShortTriangleList(buffer, byteOffset, count, mesh) {
        let offset = byteOffset;

        for (let i = 0; i < count; i+=3) {
            const p0 = buffer.readUInt16LE(offset);
            offset += 2;

            const p1 = buffer.readUInt16LE(offset);
            offset += 2;

            const p2 = buffer.readUInt16LE(offset);
            offset += 2;

            mesh.triangles.push([p0, p1, p2]);
        }
    }

    _readUIntTriangleList(buffer, byteOffset, count, mesh) {
        let offset = byteOffset;

        for (let i = 0; i < count; i+=3) {
            const p0 = buffer.readUInt32LE(offset);
            offset += 4;

            const p1 = buffer.readUInt32LE(offset);
            offset += 4;

            const p2 = buffer.readUInt32LE(offset);
            offset += 4;

            mesh.triangles.push([p0, p1, p2]);
        }
    }

    _readIndices(gltfPrimitive, n64Mesh) {
        const accessor = this.gltf.accessors[gltfPrimitive.indices];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const buffer = this._getBuffer(bufferView.buffer);

        switch (accessor.componentType) {
            case 5123:
                this._readUShortTriangleList(buffer, bufferView.byteOffset, accessor.count, n64Mesh);
                break;
            case 5125:
                this._readUIntTriangleList(buffer, bufferView.byteOffset, accessor.count, n64Mesh);
                break;

            default:
                throw new Error(`Unknown Component Type: ${accessor.componentType}`);
        }
    }

}

module.exports = GLTFLoader;