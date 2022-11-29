const N64Primitive = require("./N64Primitive");
const N64Defs = require("./N64Defs");
const N64ImageWriter = require("./N64ImageWriter");
const N64Slicer = require("./N64Slicer");
const DisplayList = require("./DisplayList");
const VertexBuffer = require("./VertexBuffer");
const Bounding = require("./Bounding");

const path  = require("path");
const fs = require("fs")


/**
 * This class represents the header of the Mesh data.
 * It is read first and describes the counts of the various components of the mesh.
 * */
class MeshInfo {
    primitiveCount = 0;
    vertexCount = 0;
    displayListCount = 0;
    vertexPointerDataSize = 0;
    bounding = null;

    get buffer() {
        const buff = Buffer.alloc(16 + Bounding.SizeOf);
        let index = 0;

        index = buff.writeUInt32BE(this.primitiveCount, index);
        index = buff.writeUInt32BE(this.vertexCount, index);
        index = buff.writeUInt32BE(this.displayListCount, index);
        index = buff.writeUInt32BE(this.vertexPointerDataSize, index);
        index = this.bounding.writeToBuffer(buff, index);

        return buff;
    }
}

/**
 * This class represents a single primitive in the mesh.
 * It contains indices into the various mesh-level arrays
 */
class PrimitiveInfo {
    verticesBufferIndex;
    displayListBufferIndex;
    material; // index into mesh's material array
    jointIndex;

    get buffer() {
        const buff = Buffer.alloc(16)

        let index = 0;
        index = buff.writeUInt32BE(this.verticesBufferIndex, index);
        index = buff.writeUInt32BE(this.displayListBufferIndex, index);
        index = buff.writeUInt32BE(this.material, index);
        index = buff.writeUInt32BE(this.jointIndex, index);

        return buff;
    }
}

class ResourcesInfo {
    imageCount;
    textureCount;
    materialCount;

    constructor(resources) {
        this.imageCount = resources.images.length;
        this.textureCount = resources.textures.length;
        this.materialCount = resources.materials.length;
    }

    get buffer() {
        const buff = Buffer.alloc(12);

        let index = 0;
        index = buff.writeUInt32BE(this.imageCount, index);
        index = buff.writeUInt32BE(this.textureCount, index);
        index = buff.writeUInt32BE(this.materialCount, index);

        return buff;
    }
}

async function writeMeshResources(resources, file) {
    const resourceInfo = new ResourcesInfo(resources);

    fs.writeSync(file, resourceInfo.buffer);

    for (const image of resources.images) {
        fs.writeSync(file, image.assetBuffer);
    }

    for (const texture of resources.textures) {
        fs.writeSync(file, texture.buffer);
    }

    for (const material of resources.materials) {
        fs.writeSync(file, material.buffer);
    }
}

async function writeStaticMesh(mesh, outputDir, archive) {
    const destPath = path.join(outputDir, `${mesh.name}.mesh`);

    const file = fs.openSync(destPath, "w");

    if (mesh.resources) {
        await writeMeshResources(mesh.resources, file);
    }

    writeStaticMeshData(mesh, file);

    fs.closeSync(file);
    await archive.add(destPath, "mesh");
}

function writeStaticMeshData(mesh, file) {
    const meshInfo = new MeshInfo();
    meshInfo.primitiveCount = mesh.primitives.length;
    meshInfo.vertexPointerDataSize = mesh.primitives.length * 4;
    meshInfo.bounding = mesh.bounding;

    const primitiveInfos = [];
    const vertexBuffers = [];
    const displayListBuffers = [];
    const vertexPointerBuffers = []
    const vertexPointerCountBuffer = Buffer.alloc(mesh.primitives.length * 4); // holds number of vertex pointer indices per primitive

    for (let i = 0; i <  mesh.primitives.length; i++) {
        const primitive = mesh.primitives[i];

        const primitiveInfo = new PrimitiveInfo();
        primitiveInfo.verticesBufferIndex = meshInfo.vertexCount; // the index for this primitive is set to the total size of the mesh's vertices buffer added thus far
        primitiveInfo.displayListBufferIndex = meshInfo.displayListCount; // the index for this primitive is set to the total size of the mesh's display list buffer this far
        primitiveInfo.material = primitive.material;
        primitiveInfo.jointIndex = primitive.jointIndices ? primitive.jointIndices[0] : N64Primitive.NoJoint; // used for skinning, refer to N64Mesh.splitPrimitivesForSkinning
        primitiveInfos.push(primitiveInfo);

        // slice the vertices into chunks that can fit into N64 vertex cache
        const slices = N64Slicer.slice(primitive);

        // generate the display list for rendering the primitive geometry
        const vertexBuffer = VertexBuffer.createVertexBuffer(slices, primitive.hasNormals);
        const {displayList, vertexPointers, vertexCommandIndices} = primitive.elementType === N64Primitive.ElementType.Triangles ?
            DisplayList.createTriangleDisplayListBuffer(slices) : DisplayList.createLineDisplayListBuffer(slices);

        // update the mesh info totals
        meshInfo.vertexCount += vertexBuffer.length / N64Defs.SizeOfVtx;
        meshInfo.displayListCount += displayList.length / N64Defs.SizeOfGfx;
        meshInfo.vertexPointerDataSize += vertexPointers.length;
        vertexPointerCountBuffer.writeUInt32BE(vertexPointers.length / 4, i * 4); // vertexPointers is a binary buffer of uint 32's so we divide to get the actual count of items in the buffer

        vertexBuffers.push(vertexBuffer);
        displayListBuffers.push(displayList);
        vertexPointerBuffers.push(vertexPointers);
    }

    fs.writeSync(file, meshInfo.buffer);

    for (const buffer of vertexBuffers)
        fs.writeSync(file, buffer);

    for (const buffer of displayListBuffers)
        fs.writeSync(file, buffer);

    for (const primitiveInfo of primitiveInfos) {
        fs.writeSync(file, primitiveInfo.buffer)
    }

    fs.writeSync(file, vertexPointerCountBuffer);
    for (const buffer of vertexPointerBuffers)
        fs.writeSync(file, buffer);
}

module.exports = {
    writeStaticMesh: writeStaticMesh,
    writeStaticMeshData: writeStaticMeshData,
    writeMeshResources: writeMeshResources
};
