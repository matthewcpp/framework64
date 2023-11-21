const fs = require("fs");
const path = require("path");

const MaterialBundle = require("../gltf/MaterialBundle");

const MaterialBundleWriter = require("./MaterialBundleWriter");

const Bounding = require("../gltf/Bounding");
const Primitive = require("../gltf/Primitive");
const GLTFVertexIndex = require("../gltf/GLTFVertexIndex");

/** Writes a self contained static mesh to file.
 *  Precondition: this gltf data should contain at least 1 mesh.
 */
async function writeStaticMesh(staticMesh, destPath) {
    const file = fs.openSync(destPath, "w");
    await writeStaticMeshToFile(staticMesh, file)
    fs.closeSync(file);
}

async function writeStaticMeshToFile(staticMesh, file) {
    if (staticMesh.materialBundle === null) {
        throw new Error("Error writing static mesh: no material bundle present on mesh.");
    }

    await _writeMeshToFile(staticMesh, staticMesh.materialBundle, file);
}

async function writeMeshData(mesh, materialBundle, file) {
    if (mesh.materialBundle != null) {
        throw new Error("Error writing mesh data: unexpected material bundle present on mesh.");
    }
    await _writeMeshToFile(mesh, materialBundle, file);
}

async function _writeMeshToFile(mesh, materialBundle, file) {
    _writeMeshInfo(mesh, file)

    if (mesh.materialBundle) {
        const gltfData = mesh.materialBundle.gltfData;
        const images = await MaterialBundleWriter.createDesktopImages(gltfData);
        await MaterialBundleWriter.write(materialBundle, images, gltfData, file);
    }

    for (const primitive of mesh.primitives) {
        const primitiveInfo = new PrimitiveInfo(primitive, materialBundle);
        fs.writeSync(file, primitiveInfo.buffer);

        const funcs = [];
        const buffers = [];

        if (primitive.hasPositions) {
            funcs.push(_writePosition);
            buffers.push(Buffer.alloc(primitive.vertices.length * 3 * 4));
        }

        if (primitive.hasNormals) {
            funcs.push(_writeNormal);
            buffers.push(Buffer.alloc(primitive.vertices.length * 3 * 4));
        }

        if (primitive.hasTexCoords) {
            funcs.push(_writeTexCoord);
            buffers.push(Buffer.alloc(primitive.vertices.length * 2 * 4));
        }

        if (primitive.hasVertexColors) {
            funcs.push(_writeVertexColor);
            buffers.push(Buffer.alloc(primitive.vertices.length * 4 * 4));
        }

        for (let i = 0; i < primitive.vertices.length; i++) {
            for(let j = 0; j < funcs.length; j++) {
                funcs[j](primitive.vertices[i], i , buffers[j]);
            }
        }

        for (buffer of buffers) {
            fs.writeSync(file, buffer);
        }

        const elementCount = primitive.elementType ==  Primitive.ElementType.Triangles ?  3 : 2;
        const elementBuffer = Buffer.alloc(primitive.elements.length * 2 * elementCount);
        let elementBufferIndex = 0;
        for (const element of primitive.elements) {
            for(const index of element) {
                elementBufferIndex = elementBuffer.writeUInt16LE(index, elementBufferIndex);
            }
        }

        fs.writeSync(file, elementBuffer);
    }
}

/** The order that data is written in this function needs to match up with MeshInfo in desktop/mesh.cpp */
function _writeMeshInfo(mesh, file) {
    const buffer = Buffer.alloc(8 + Bounding.SizeOf)

    let index = 0;
    index = buffer.writeUint32LE(mesh.primitives.length, index);
    index = buffer.writeUint32LE(mesh.hasMaterialBundle ? 1 : 0, index);
    mesh.bounding.writeToBufferLE(buffer, index);

    fs.writeSync(file, buffer);
}

function _writePosition(vertex, vertexIndex, buffer) {
    let bufferIndex = vertexIndex * 3 * 4;
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.PositionX], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.PositionY], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.PositionZ], bufferIndex);
}

function _writeNormal(vertex, vertexIndex, buffer) {
    let bufferIndex = vertexIndex * 3 * 4;
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.NormalX], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.NormalY], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.NormalZ], bufferIndex);
}

function _writeTexCoord(vertex, vertexIndex, buffer) {
    let bufferIndex = vertexIndex * 2 * 4;
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.TexCoordU], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.TexCoordV], bufferIndex);
}

function _writeVertexColor(vertex, vertexIndex, buffer) {
    let bufferIndex = vertexIndex * 4 * 4;
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.ColorR], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.ColorG], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.ColorB], bufferIndex);
    bufferIndex = buffer.writeFloatLE(vertex[GLTFVertexIndex.ColorA], bufferIndex);
}

/// This needs to be kept in sync with desktop/mesh.h
class VertexAttributes{
    static None = 0;
    static Positions = 1;
    static Normals = 2;
    static TexCoords = 4;
    static VertexColors = 8;
}

/// This needs to be kept in sync with desktop/mesh.cpp PrimitiveInfo
class PrimitiveInfo {
    primitive = null;

    static SizeOf = 24 + Bounding.SizeOf

    vertexCount = 0;
    vertexAttributes = VertexAttributes.None;
    elementCount = 0;
    mode = Primitive.ElementType.Triangles;
    materialIndex = Primitive.NoMaterial;
    jointIndex = Primitive.NoJoint

    constructor(primitive, materialBundle) {
        this.primitive = primitive;
        this.vertexCount = primitive.vertices.length;
        this.vertexAttributes = VertexAttributes.Positions;
        this.elementCount = primitive.elements.length;
        this.mode = primitive.elementType;
        this.materialIndex = materialBundle.getBundledMaterialIndex(primitive.material);
        this.jointIndex = primitive.jointIndices ? primitive.jointIndices[0] : Primitive.NoJoint;

        if (primitive.hasNormals) {
            this.vertexAttributes |= VertexAttributes.Normals;
        }

        if (primitive.hasVertexColors) {
            this.vertexAttributes |= VertexAttributes.VertexColors;
        }

        if (primitive.hasTexCoords) {
            this.vertexAttributes |= VertexAttributes.TexCoords;
        }
    }

    get buffer() {
        const buffer = Buffer.alloc(PrimitiveInfo.SizeOf);

        let index = 0;
        index = buffer.writeUint32LE(this.vertexCount, index);
        index = buffer.writeUint32LE(this.vertexAttributes, index);
        index = buffer.writeUint32LE(this.elementCount, index);
        index = buffer.writeUint32LE(this.mode, index);
        index = buffer.writeUint32LE(this.materialIndex, index);
        index = buffer.writeUint32LE(this.jointIndex, index);
        this.primitive.bounding.writeToBufferLE(buffer, index);

        return buffer;
    }
}

module.exports = {
    writeStaticMesh: writeStaticMesh,
    writeStaticMeshToFile: writeStaticMeshToFile,
    writeMeshData: writeMeshData
};