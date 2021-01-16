const N64Model = require("./N64Model")
const N64Mesh = require("./N64Mesh")

const glMatrix = require("gl-matrix");

const fs = require("fs");
const path = require("path");

class GLTFLoader {
    constructor() {
        this.loadedBuffers = new Map();
        this.globalScale = 1.0;
        this.gltfPath = null;
        this.gltf = null;
        this.transform = glMatrix.mat4.create();
        this.bakeTransform = false;
        this.model = null;
    }

    load(gltfPath) {
        this.gltfPath = gltfPath;
        this.gltf = JSON.parse(fs.readFileSync(gltfPath, {encoding: "utf8"}));

        const modelName = path.basename(gltfPath, ".gltf");
        this.model = new N64Model(modelName);

        const scene = this.gltf.scenes[this.gltf.scene];

        for (const nodeIndex of scene.nodes) {
            const gltfNode = this.gltf.nodes[nodeIndex];
            this._getTransform(gltfNode);

            const gltfMesh = this.gltf.meshes[gltfNode.mesh];

            for (const gltfPrimitive of gltfMesh.primitives) {
                // only support triangles for now
                if (gltfPrimitive.hasOwnProperty("mode") && gltfPrimitive.mode != 4)
                    continue;

                const n64Mesh = new N64Mesh();
                this.model.meshes.push(n64Mesh);

                this._readPositions(gltfPrimitive, n64Mesh);
                this._readTexCoords(gltfPrimitive, n64Mesh);
                this._readMaterial(gltfPrimitive, n64Mesh);

                this._readIndices(gltfPrimitive, n64Mesh);
            }
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
                parseInt(buffer.readFloatLE(offset) * this.globalScale),
                parseInt(buffer.readFloatLE(offset + 4) * this.globalScale),
                parseInt(buffer.readFloatLE(offset + 8) * this.globalScale)
            ];

            if (this.bakeTransform) {
                glMatrix.vec3.transformMat4(position, position, this.transform);
            }

            n64Mesh.vertices.push([...position, 0]);
            offset += byteStride;
        }

        n64Mesh.bounding.min = accessor.min.slice();
        n64Mesh.bounding.max = accessor.max.slice();
    }



    _readTexCoords(gltfPrimitive, n64Mesh) {
        /*
        inline fixed_point_t float_to_fixed(double input)
        {
            return (fixed_point_t)(round(input * (1 << FIXED_POINT_FRACTIONAL_BITS)));
        }
        */

        for (const vertex of n64Mesh.vertices) {
            vertex.push(0, 0);
        }
    }

    _readMaterial(gltfPrimitive, n64Mesh) {
        const material = this.gltf.materials[gltfPrimitive.material];
        const baseColor = material.pbrMetallicRoughness.baseColorFactor;
        const rgba = [parseInt(baseColor[0] * 255), parseInt(baseColor[1] * 255), parseInt(baseColor[2] * 255), parseInt(baseColor[3] * 255)];

        for (const vertex of n64Mesh.vertices) {
            vertex.push(...rgba);
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