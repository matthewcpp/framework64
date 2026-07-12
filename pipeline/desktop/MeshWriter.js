

const MaterialBundleWriter = require("./MaterialBundleWriter");
const WriteInterface = require("../WriteInterface");

const Bounding = require("../gltf/Bounding");
const Primitive = require("../gltf/Primitive");
const GLTFVertexIndex = require("../gltf/GLTFVertexIndex");
const VertexAttributes = require("../gltf/VertexAttributes");

const fs = require("fs");

class DesktopMeshWriter {
    _materialBundleWriter;

    constructor(materialBundleWriter) {
        this._materialBundleWriter = materialBundleWriter;
    }

    /** Writes a self contained static mesh to file.
     *  Precondition: this gltf data should contain at least 1 mesh.
     */
    async writeStaticMesh(environment, staticMesh, destPath) {
        const file = fs.openSync(destPath, "w");
        await this.writeStaticMeshToFile(environment, staticMesh, file)
        fs.closeSync(file);
    }

    async writeStaticMeshToFile(environment, staticMesh, file) {
        if (staticMesh.materialBundle === null) {
            throw new Error("Error writing static mesh: no material bundle present on mesh.");
        }

        await this._writeMeshToFile(environment, staticMesh, staticMesh.materialBundle, file);
    }

    async writeMeshData(environment, mesh, materialBundle, images, file) {
        if (mesh.materialBundle != null) {
            throw new Error("Error writing mesh data: unexpected material bundle present on mesh.");
        }
        await this._writeMeshToFile(environment, mesh, materialBundle, file);
    }

    async _writeMeshToFile(environment, mesh, materialBundle, file) {
        const writer = WriteInterface.littleEndian();

        this._writeMeshInfo(mesh, writer, file)

        if (mesh.materialBundle) {
            const gltfData = mesh.materialBundle.gltfData;
            const images = await this._materialBundleWriter.createImages(gltfData);
            await this._materialBundleWriter.write(materialBundle, images, gltfData, file);
        }

        this._writeMeshData(environment, mesh, materialBundle, writer, file);
    }

    /** The order that data is written in this function needs to match up with MeshInfo in desktop/mesh.cpp */
    _writeMeshInfo(mesh, writer, file) {
        const buffer = Buffer.alloc(8 + Bounding.SizeOf)

        let index = 0;
        index = writer.writeUInt32(buffer, mesh.primitives.length, index);
        index = writer.writeUInt32(buffer, mesh.hasMaterialBundle ? 1 : 0, index);
        mesh.bounding.write(writer, buffer, index);

        fs.writeSync(file, buffer);
    }

    _writeMeshData(environment, mesh, materialBundle, writer, file) {
        for (const primitive of mesh.primitives) {
            this._writePimitiveInfo(primitive, materialBundle, writer, file);

            const funcs = [];
            const buffers = [];

            if (primitive.hasPositions) {
                funcs.push(DesktopMeshWriter._writePosition);
                buffers.push(Buffer.alloc(primitive.vertices.length * 3 * 4));
            }

            if (primitive.hasNormals) {
                funcs.push(DesktopMeshWriter._writeNormal);
                buffers.push(Buffer.alloc(primitive.vertices.length * 3 * 4));
            }

            if (primitive.hasTexCoords) {
                funcs.push(DesktopMeshWriter._writeTexCoord);
                buffers.push(Buffer.alloc(primitive.vertices.length * 2 * 4));
            }

            if (primitive.hasVertexColors) {
                // TODO: can we move this out of here?
                if (environment.platform === "desktop" || environment.platform === "web") {
                    funcs.push(DesktopMeshWriter._writeVertexColorFloat);
                    buffers.push(Buffer.alloc(primitive.vertices.length * 4 * 4));
                }
                else {
                    funcs.push(DesktopMeshWriter._writeVertexColorUnsignedByte);
                    buffers.push(Buffer.alloc(primitive.vertices.length * 4));
                }
            }

            for (let i = 0; i < primitive.vertices.length; i++) {
                for(let j = 0; j < funcs.length; j++) {
                    funcs[j](primitive.vertices[i], i , buffers[j], writer);
                }
            }

            for (const buffer of buffers) {
                fs.writeSync(file, buffer);
            }

            const elementCount = primitive.elementType ==  Primitive.ElementType.Triangles ?  3 : 2;
            const elementBuffer = Buffer.alloc(primitive.elements.length * 2 * elementCount);
            let elementBufferIndex = 0;
            for (const element of primitive.elements) {
                for(const index of element) {
                    elementBufferIndex = writer.writeUInt16(elementBuffer, index, elementBufferIndex);
                }
            }

            fs.writeSync(file, elementBuffer);
        }
    }

    /// This needs to be kept in sync with:
    /// desktop/mesh.cpp PrimitiveInfo
    /// gl/gl_mesh.c PrimitiveInfo
    _writePimitiveInfo(primitive, materialBundle, writer, file) {
        const buffer = Buffer.alloc(24 + Bounding.SizeOf);

        let vertexAttributes = VertexAttributes.Positions;

        if (primitive.hasNormals) {
            vertexAttributes |= VertexAttributes.Normals;
        }

        if (primitive.hasVertexColors) {
            vertexAttributes |= VertexAttributes.VertexColors;
        }

        if (primitive.hasTexCoords) {
            vertexAttributes |= VertexAttributes.TexCoords;
        }

        let index = 0;
        index = writer.writeUInt32(buffer, primitive.vertices.length, index);
        index = writer.writeUInt32(buffer, vertexAttributes, index);
        index = writer.writeUInt32(buffer, primitive.elements.length, index);
        index = writer.writeUInt32(buffer, primitive.elementType, index);
        index = writer.writeUInt32(buffer, materialBundle.getBundledMaterialIndex(primitive.material), index);
        index = writer.writeUInt32(buffer, primitive.jointIndices ? primitive.jointIndices[0] : Primitive.NoJoint, index);
        primitive.bounding.write(writer, buffer, index);

        fs.writeSync(file, buffer);
    }

    static _writePosition(vertex, vertexIndex, buffer, writer) {
        let bufferIndex = vertexIndex * 3 * 4;
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.PositionX], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.PositionY], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.PositionZ], bufferIndex);
    }

    static _writeNormal(vertex, vertexIndex, buffer, writer) {
        let bufferIndex = vertexIndex * 3 * 4;
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.NormalX], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.NormalY], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.NormalZ], bufferIndex);
    }

    static _writeTexCoord(vertex, vertexIndex, buffer, writer) {
        let bufferIndex = vertexIndex * 2 * 4;
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.TexCoordU], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.TexCoordV], bufferIndex);
    }

    static _writeVertexColorFloat(vertex, vertexIndex, buffer, writer) {
        let bufferIndex = vertexIndex * 4 * 4;
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorR], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorG], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorB], bufferIndex);
        bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorA], bufferIndex);
    }

    static _writeVertexColorUnsignedByte(vertex, vertexIndex, buffer, writer) {
        let bufferIndex = vertexIndex * 4;
        bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorR] * 255), bufferIndex);
        bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorG] * 255), bufferIndex);
        bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorB] * 255), bufferIndex);
        bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorA] * 255), bufferIndex);
    }
};

module.exports = DesktopMeshWriter;
