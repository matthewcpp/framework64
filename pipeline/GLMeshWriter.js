const Bounding = require("./gltf/Bounding");
const Primitive = require("./gltf/Primitive");
const GLTFVertexIndex = require("./gltf/GLTFVertexIndex");

const fs = require("fs");
const BuildInfo = require("./BuildInfo");

/** The order that data is written in this function needs to match up with MeshInfo in desktop/mesh.cpp */
function writeMeshInfo(mesh, writer, file) {
    const buffer = Buffer.alloc(8 + Bounding.SizeOf)

    let index = 0;
    index = writer.writeUInt32(buffer, mesh.primitives.length, index);
    index = writer.writeUInt32(buffer, mesh.hasMaterialBundle ? 1 : 0, index);
    mesh.bounding.write(writer, buffer, index);

    fs.writeSync(file, buffer);
}

function writeMeshData(mesh, materialBundle, writer, file) {
    for (const primitive of mesh.primitives) {
        _writePimitiveInfo(primitive, materialBundle, writer, file);

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
            if (BuildInfo.platform == "desktop") {
                funcs.push(_writeVertexColorFloat);
                buffers.push(Buffer.alloc(primitive.vertices.length * 4 * 4));
            }
            else {
                funcs.push(_writeVertexColorUnsignedByte);
                buffers.push(Buffer.alloc(primitive.vertices.length * 4));
            }
        }

        for (let i = 0; i < primitive.vertices.length; i++) {
            for(let j = 0; j < funcs.length; j++) {
                funcs[j](primitive.vertices[i], i , buffers[j], writer);
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
                elementBufferIndex = writer.writeUInt16(elementBuffer, index, elementBufferIndex);
            }
        }

        fs.writeSync(file, elementBuffer);
    }
}

/// This needs to be kept in sync with:
/// desktop/mesh.cpp PrimitiveInfo
/// gl/gl_mesh.c PrimitiveInfo
function _writePimitiveInfo(primitive, materialBundle, writer, file) {
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

function _writePosition(vertex, vertexIndex, buffer, writer) {
    let bufferIndex = vertexIndex * 3 * 4;
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.PositionX], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.PositionY], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.PositionZ], bufferIndex);
}

function _writeNormal(vertex, vertexIndex, buffer, writer) {
    let bufferIndex = vertexIndex * 3 * 4;
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.NormalX], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.NormalY], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.NormalZ], bufferIndex);
}

function _writeTexCoord(vertex, vertexIndex, buffer, writer) {
    let bufferIndex = vertexIndex * 2 * 4;
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.TexCoordU], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.TexCoordV], bufferIndex);
}

function _writeVertexColorFloat(vertex, vertexIndex, buffer, writer) {
    let bufferIndex = vertexIndex * 4 * 4;
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorR], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorG], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorB], bufferIndex);
    bufferIndex = writer.writeFloat(buffer, vertex[GLTFVertexIndex.ColorA], bufferIndex);
}

function _writeVertexColorUnsignedByte(vertex, vertexIndex, buffer, writer) {
    let bufferIndex = vertexIndex * 4;
    bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorR] * 255), bufferIndex);
    bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorG] * 255), bufferIndex);
    bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorB] * 255), bufferIndex);
    bufferIndex = buffer.writeUInt8(Math.round(vertex[GLTFVertexIndex.ColorA] * 255), bufferIndex);
}

/// This needs to be kept in sync with desktop/mesh.h
class VertexAttributes{
    static None = 0;
    static Positions = 1;
    static Normals = 2;
    static TexCoords = 4;
    static VertexColors = 8;
}

module.exports = {
    writeMeshInfo: writeMeshInfo,
    writeMeshData: writeMeshData
}
