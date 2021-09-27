const N64Primitive = require("./N64Primitive");
const N64Defs = require("./N64Defs");
const N64SpriteWriter = require("./N64SpriteWriter");
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
    materialCount = 0;
    textureCount = 0;
    vertexCount = 0;
    displayListCount = 0;
    vertexPointerDataSize = 0;
    bounding = null;

    get buffer() {
        const buff = Buffer.alloc(24 + Bounding.SizeOf);
        let index = 0;

        index = buff.writeUInt32BE(this.primitiveCount, index);
        index = buff.writeUInt32BE(this.materialCount, index);
        index = buff.writeUInt32BE(this.textureCount, index);
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
    bounding;
    verticesBufferIndex;
    displayListBufferIndex;
    material;// index into mesh's material array

    get buffer() {
        const buff = Buffer.alloc(36)

        let index = this.bounding.writeToBuffer(buff, 0);
        index = buff.writeUInt32BE(this.verticesBufferIndex, index);
        index = buff.writeUInt32BE(this.displayListBufferIndex, index);
        index = buff.writeUInt32BE(this.material, index);

        return buff;
    }
}

/**
 * Textures are written out as individual assets into the archive.
 * This function will return a buffer of the asset indices which corresponds to the image array of the mesh.
 */
function prepareTextures(mesh, outputDir, archive) {
    const textureAssetIndicesBuffer = Buffer.alloc(mesh.images.length * 4);
    let bufferIndex = 0;

    for (const image of mesh.images) {
        const texturePath = path.join(outputDir, `${image.name}.sprite`);
        let entry = archive.entries.get(texturePath);

        if (!entry){
            entry = archive.add(texturePath, "sprite");
            N64SpriteWriter.write(image, 1, 1, texturePath);
        }


        bufferIndex = textureAssetIndicesBuffer.writeUInt32BE(entry.index, bufferIndex);
    }

    return textureAssetIndicesBuffer;
}

function writeStaticMesh(mesh, outputDir, archive) {
    const destPath = path.join(outputDir, `${mesh.name}.mesh`);
    archive.add(destPath, "mesh");

    mesh.setMaterialShadingModes();

    const meshInfo = new MeshInfo();
    meshInfo.primitiveCount = mesh.primitives.length;
    meshInfo.materialCount = mesh.materials.length;
    meshInfo.textureCount = mesh.images.length;
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
        primitiveInfo.bounding = primitive.bounding;
        primitiveInfo.verticesBufferIndex = meshInfo.vertexCount; // the index for this primitive is set to the total size of the mesh's vertices buffer added thus far
        primitiveInfo.displayListBufferIndex = meshInfo.displayListCount; // the index for this primitive is set to the total size of the mesh's display list buffer this far
        primitiveInfo.material = primitive.material;

        primitiveInfos.push(primitiveInfo);

        // slice the vertices into chunks that can fit into N64 vertex cache
        const slices = N64Slicer.slice(primitive);

        // generate the display list for rendering the primitive geometry
        const vertexBuffer = VertexBuffer.createVertexBuffer(slices, primitive.hasNormals);
        const {displayList, vertexPointers} = primitive.elementType === N64Primitive.ElementType.Triangles ?
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

    const file = fs.openSync(destPath, "w");
    fs.writeSync(file, meshInfo.buffer);

    for (const buffer of vertexBuffers)
        fs.writeSync(file, buffer);

    for (const buffer of displayListBuffers)
        fs.writeSync(file, buffer);

    for (const material of mesh.materials) {
        fs.writeSync(file, material.buffer);
    }

    for (const primitiveInfo of primitiveInfos) {
        fs.writeSync(file, primitiveInfo.buffer)
    }

    fs.writeSync(file, vertexPointerCountBuffer);
    for (const buffer of vertexPointerBuffers)
        fs.writeSync(file, buffer);

    if (mesh.images.length > 0) {
        const textureAssetIndices = prepareTextures(mesh, outputDir, archive);
        fs.writeSync(file, textureAssetIndices);
    }

    fs.closeSync(file);
}

module.exports = {
    writeStaticMesh: writeStaticMesh
};
