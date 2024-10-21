const Bounding = require("../gltf/Bounding")
const GLTFVertexIndex = require("../gltf/GLTFVertexIndex");
const MaterialBundleWrtier = require("./MaterialBundleWriter");
const Primitive = require("../gltf/Primitive");
const VertexAttributes = require("../gltf/VertexAttributes");

const fs = require("fs");
const path = require("path");

async function writeStaticMesh(staticMesh /* Mesh.js */, dfsAssets, dfsDir, meshFileName) {
    const meshDestPath = path.join(dfsDir, meshFileName);
    const file = fs.openSync(meshDestPath, "w");

    await writeStaticMeshToFile(staticMesh, dfsAssets, dfsDir, file);

    fs.closeSync(file);
}

async function writeStaticMeshToFile(staticMesh, dfsAssets, dfsDir, file) {
    const libdragonImages = await MaterialBundleWrtier.tempWriteLibdragonImages(staticMesh.materialBundle, dfsAssets, dfsDir);

    // Write mesh info
    // this needs to corespond with libdragon_mesh.c MeshInfo
    const meshInfoBuffer = Buffer.alloc(8);
    meshInfoBuffer.writeUint32BE(staticMesh.primitives.length, 0);
    meshInfoBuffer.writeUint32BE(1 /*indicated material buffer*/, 4);
    fs.writeSync(file, meshInfoBuffer);

    MaterialBundleWrtier.write(staticMesh.materialBundle, libdragonImages, staticMesh.materialBundle.gltfData, file);

    // TODO: change this order when mesh loading works
    // write the bounding
    const boundingBuffer = Buffer.alloc(Bounding.SizeOf);
    staticMesh.bounding.writeToBufferBE(boundingBuffer, 0);
    fs.writeSync(file, boundingBuffer);

    // this buffer will hold the bundled index of the material for each primitive
    const materialIndexBuffer = Buffer.alloc(4 * staticMesh.primitives.length);
    for (let i = 0; i < staticMesh.primitives.length; i++) {
        materialIndexBuffer.writeUInt32BE(staticMesh.materialBundle.getBundledMaterialIndex(staticMesh.primitives[i].material), i * 4);
    }
    fs.writeSync(file, materialIndexBuffer);
    
    for (const primitive /*Primitive.js */ of staticMesh.primitives) {
        let vertexAttributes = VertexAttributes.None;
        let vertexSizeInBytes = 0;
        let writerFuncs = [];

        if (primitive.hasPositions) {
            vertexAttributes |= VertexAttributes.Positions;
            vertexSizeInBytes += 4 * 3;
            writerFuncs.push(writePositions);
        }

        if (primitive.hasNormals) {
            vertexAttributes |= VertexAttributes.Normals;
            vertexSizeInBytes += 4 * 3;
            writerFuncs.push(writeNormals);
        }

        if (primitive.hasTexCoords) {
            vertexAttributes |= VertexAttributes.TexCoords;
            vertexSizeInBytes += 4 * 2;
            writerFuncs.push(writeTexCoords);
        }

        if (primitive.hasVertexColors) {
            vertexAttributes |= VertexAttributes.VertexColors;
            vertexSizeInBytes += 4 * 1;
            writerFuncs.push(writeVertexColors);
        }

        const vertexDataSize = vertexSizeInBytes * primitive.vertices.length;
        const vertexDataBuffer = Buffer.alloc(vertexDataSize);

        let index = 0;
        for (const vertex of primitive.vertices) {
            for (const writerFunc of writerFuncs) {
                index = writerFunc(vertex, vertexDataBuffer, index);
            }
        }

        // convert and write element data
        const elementCount = (primitive.elementType ==  Primitive.ElementType.Triangles ?  3 : 2) * primitive.elements.length;
        const elementBuffer = Buffer.alloc(elementCount * 2);
        let elementBufferIndex = 0;
        for (const element of primitive.elements) {
            for(const index of element) {
                elementBufferIndex = elementBuffer.writeUInt16BE(index, elementBufferIndex);
            }
        }

        // this writing code needs to match up with Primitive Info in libdragon_mesh.c
        const primitiveInfoBuffer = Buffer.alloc(6 * 4);
        primitiveInfoBuffer.writeUInt32BE(primitive.vertices.length), 0;
        primitiveInfoBuffer.writeUInt32BE(vertexDataSize, 4);
        primitiveInfoBuffer.writeUInt32BE(vertexAttributes, 8);
        primitiveInfoBuffer.writeUInt32BE(primitive.elementType /*mode*/, 12);
        primitiveInfoBuffer.writeUInt32BE(elementCount, 16);
        primitiveInfoBuffer.writeUInt32BE(primitive.jointIndices ? primitive.jointIndices[0] : Primitive.NoJoint, 20);

        fs.writeSync(file, primitiveInfoBuffer);
        fs.writeSync(file, vertexDataBuffer);
        fs.writeSync(file, elementBuffer);
    }
}

function writePositions(vertex, buffer, index) {
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.PositionX], index);
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.PositionY], index);
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.PositionZ], index);

    return index;
}

function writeNormals(vertex, buffer, index) {
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.NormalX], index);
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.NormalY], index);
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.NormalZ], index);

    return index;
}

function writeTexCoords(vertex, buffer, index) {
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.TexCoordU], index);
    index = buffer.writeFloatBE(vertex[GLTFVertexIndex.TexCoordV], index);

    return index;
}

function writeVertexColors(vertex, buffer, index) {
    index = buffer.writeUint8(Math.min(vertex[GLTFVertexIndex.ColorR] * 255, 255), index);
    index = buffer.writeUint8(Math.min(vertex[GLTFVertexIndex.ColorG] * 255, 255), index);
    index = buffer.writeUint8(Math.min(vertex[GLTFVertexIndex.ColorB] * 255, 255), index);
    index = buffer.writeUint8(Math.min(vertex[GLTFVertexIndex.ColorA] * 255, 255), index);

    return index;
}

module.exports = {
    writeStaticMesh: writeStaticMesh,
    writeStaticMeshToFile: writeStaticMeshToFile
}