const N64Image = require("./N64Image");
const N64Material = require("./N64Material");
const N64Mesh = require("./N64Mesh");
const N64ImageWriter = require("./N64ImageWriter");
const N64SpriteWriter = require("./N64SpriteWriter");
const DisplayList = require("./DisplayList");
const Bounding = require("./Bounding");

const path  = require("path");
const fs = require("fs")

const SizeOfVtx = 16; // size (in bytes) of a single vertex
const SizeOfGfx = 8; // size (in bytes) of a single entry in a display list

function createVertexBuffer(slices, hasNormals) {
    const vertexList = [];
    for (const slice of slices)
        vertexList.push(...slice.vertices)

    const vertexBuffer = Buffer.alloc(vertexList.length * SizeOfVtx);
    let bufferOffset = 0;

    for (const vertex of vertexList) {
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[0], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[1], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[2], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[3], bufferOffset);

        bufferOffset =  vertexBuffer.writeInt16BE(vertex[4], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[5], bufferOffset);

        if (hasNormals) {
            bufferOffset =  vertexBuffer.writeInt8(vertex[6], bufferOffset);
            bufferOffset =  vertexBuffer.writeInt8(vertex[7], bufferOffset);
            bufferOffset =  vertexBuffer.writeInt8(vertex[8], bufferOffset);
        }
        else { // vertex colors
            bufferOffset =  vertexBuffer.writeUInt8(vertex[6], bufferOffset);
            bufferOffset =  vertexBuffer.writeUInt8(vertex[7], bufferOffset);
            bufferOffset =  vertexBuffer.writeUInt8(vertex[8], bufferOffset);
        }

        bufferOffset =  vertexBuffer.writeUInt8(vertex[9], bufferOffset);
    }

    return vertexBuffer;
}

function createTriangleDisplayListBuffer(slices) {
    let displayListSize = 1; // gSPEndDisplayList
    for (const slice of slices) {
        displayListSize += 1 + slice.elements.length / 2 + slice.elements.length % 2;
    }

    const gfx = Buffer.alloc(8);

    const displayListBuffer = Buffer.alloc(displayListSize * SizeOfGfx);
    let displayListIndex = 0; // holds the current index into the display list array.

    const vertexPointerBuffer = Buffer.alloc(slices.length * 4); // holds indices of this primitive's display list that contain vertex cache pointers (gSPVertex)
    let vertexPointerBufferIndex = 0;

    let vertexBufferOffset = 0; // holds the offset into the vertex buffer for this primitive.  Incremented every slice

    for (const slice of slices) {
        // writes the index of the gSPVertex call of the displaylist into the list of items that need to be fixed up when loaded at runtime
        vertexPointerBufferIndex = vertexPointerBuffer.writeInt32BE(displayListIndex, vertexPointerBufferIndex);

        // writes the relative offset into the vertex buffer for this slice.  At truntime the base address of the primitive's vertex buffer will be added to this value
        DisplayList.gSPVertex(gfx, vertexBufferOffset, slice.vertices.length, 0)
        gfx.copy(displayListBuffer, displayListIndex++ * SizeOfGfx);

        for (let i = 0; i < slice.elements.length - slice.elements.length % 2; i+= 2) {
            const t1 = slice.elements[i];
            const t2 = slice.elements[i + 1];

            DisplayList.gSP2Triangles(gfx, t1[0],t1[1],t1[2],0,t2[0],t2[1],t2[2],0);
            gfx.copy(displayListBuffer, displayListIndex++ * SizeOfGfx);
        }

        if (slice.elements.length % 2 !== 0) {
            const t1 = slice.elements[slice.elements.length - 1];
            DisplayList.gSP1Triangle(gfx, t1[0],t1[1],t1[2],0);
            gfx.copy(displayListBuffer, displayListIndex++ * SizeOfGfx);
        }

        vertexBufferOffset += slice.vertices.length * SizeOfVtx;
    }

    DisplayList.gSPEndDisplayList(gfx);
    gfx.copy(displayListBuffer, displayListIndex++ * SizeOfGfx);

    return {
        displayList: displayListBuffer,
        vertexPointers: vertexPointerBuffer,
    }
}

function createLineDisplayListBuffer(slices) {
    let displayListSize = 1; // gSPEndDisplayList
    for (const slice of slices) {
        displayListSize += 1 + slice.elements.length;
    }

    const gfx = Buffer.alloc(8);

    const displayListBuffer = Buffer.alloc(displayListSize * SizeOfGfx);
    let displayListIndex = 0; // holds the current index into the display list array.

    const vertexPointerBuffer = Buffer.alloc(slices.length * 4); // holds indices of this primitive's display list that contain vertex cache pointers (gSPVertex)
    let vertexPointerBufferIndex = 0;

    let vertexBufferOffset = 0; // holds the offset into the vertex buffer for this primitive.  Incremented every slice

    for (const slice of slices) {
        // writes the index of the gSPVertex call of the displaylist into the list of items that need to be fixed up when loaded at runtime
        vertexPointerBufferIndex = vertexPointerBuffer.writeInt32BE(displayListIndex, vertexPointerBufferIndex);

        // writes the relative offset into the vertex buffer for this slice.  At runtime the base address of the primitive's vertex buffer will be added to this value
        DisplayList.gSPVertex(gfx, vertexBufferOffset, slice.vertices.length, 0)
        gfx.copy(displayListBuffer, displayListIndex++ * SizeOfGfx);

        for (const line of slice.elements) {
            DisplayList.gSPLine3D(gfx, line[0], line[1], 0);
            gfx.copy(displayListBuffer, displayListIndex++ * SizeOfGfx);
        }

        vertexBufferOffset += slice.vertices.length * SizeOfVtx;
    }

    DisplayList.gSPEndDisplayList(gfx);
    gfx.copy(displayListBuffer, displayListIndex++ * SizeOfGfx);

    return {
        displayList: displayListBuffer,
        vertexPointers: vertexPointerBuffer,
    }
}

class MeshInfo {
    primitiveCount = 0;
    colorCount = 0;
    textureCount = 0;
    vertexCount = 0;
    displayListCount = 0;
    vertexPointerDataSize = 0;
    bounding = null;

    get buffer() {
        const buff = Buffer.alloc(24 + Bounding.SizeOf);

        buff.writeUInt32BE(this.primitiveCount, 0);
        buff.writeUInt32BE(this.colorCount, 4);
        buff.writeUInt32BE(this.textureCount, 8);
        buff.writeUInt32BE(this.vertexCount, 12);
        buff.writeUInt32BE(this.displayListCount, 16);
        buff.writeUInt32BE(this.vertexPointerDataSize, 20);
        this.bounding.writeToBuffer(buff, 24);

        return buff;
    }
}

class PrimitiveInfo {
    bounding;
    vertices;
    displayList;
    materialColor;
    materialTexture;
    materialMode;

    get buffer() {
        const buff = Buffer.alloc(44)

        let index = this.bounding.writeToBuffer(buff, 0);
        index = buff.writeUInt32BE(this.vertices, index);
        index = buff.writeUInt32BE(this.displayList, index);
        index = buff.writeUInt32BE(this.materialColor, index);
        index = buff.writeUInt32BE(this.materialTexture, index);
        index = buff.writeUInt32BE(this.materialMode, index);

        return buff;
    }
}

const ShadingMode  = {
    UnlitVertexColors: 1,
    Gouraud: 3,
    GouraudTextured: 4
}

function getShadingMode(primitive, model){
    const material = model.materials[primitive.material];

    if (primitive.hasNormals) {
        return material.texture !== N64Material.NoTexture ? ShadingMode.GouraudTextured: ShadingMode.Gouraud;
    }
    else if (primitive.hasVertexColors) {
        return ShadingMode.UnlitVertexColors;
    }

    throw new Error(`Could not determine shading mode for mesh in model: ${model.name}`);
}

// TODO: Add support for sliced sprites?
function prepareTextures(model, outputDir, archive) {
    const textureAssetIndicesBuffer = Buffer.alloc(model.images.length * 4);
    let bufferIndex = 0;

    for (const image of model.images) {
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

function write(model, outputDir, archive) {
    const modelPath = path.join(outputDir, `${model.name}.model`);
    archive.add(modelPath, "mesh");

    const meshInfo = new MeshInfo();
    meshInfo.primitiveCount = model.meshes.length;
    meshInfo.colorCount = model.materials.length;
    meshInfo.textureCount = model.images.length;
    meshInfo.vertexPointerDataSize = model.meshes.length * 4;
    meshInfo.bounding = model.bounding;

    const primitiveInfos = [];
    const vertexBuffers = [];
    const displayListBuffers = [];
    const vertexPointerBuffers = []
    const vertexPointerCountBuffer = Buffer.alloc(model.meshes.length * 4); // holds number of vertex pointer indices per primitive

    for (let i = 0; i <  model.meshes.length; i++) {
        const primitive = model.meshes[i];

        const primitiveInfo = new PrimitiveInfo();
        primitiveInfo.bounding = primitive.bounding;
        primitiveInfo.vertices = meshInfo.vertexCount;
        primitiveInfo.displayList = meshInfo.displayListCount;
        primitiveInfo.materialColor = primitive.material;
        primitiveInfo.materialTexture = model.materials[primitive.material].texture;
        primitiveInfo.materialMode = getShadingMode(primitive, model);

        primitiveInfos.push(primitiveInfo);

        // slice the vertices into chunks that can fit into N64 vertex cache
        const slices = primitive.slice();

        // generate the display list for rendering the primitive geometry
        const vertexBuffer = createVertexBuffer(slices, primitive.hasNormals);
        const {displayList, vertexPointers} = primitive.elementType === N64Mesh.ElementType.Triangles ?
            createTriangleDisplayListBuffer(slices) : createLineDisplayListBuffer(slices);

        // update the mesh info totals
        meshInfo.vertexCount += vertexBuffer.length / SizeOfVtx;
        meshInfo.displayListCount += displayList.length / SizeOfGfx;
        meshInfo.vertexPointerDataSize += vertexPointers.length;
        vertexPointerCountBuffer.writeUInt32BE(vertexPointers.length / 4, i * 4);

        vertexBuffers.push(vertexBuffer);
        displayListBuffers.push(displayList);
        vertexPointerBuffers.push(vertexPointers);
    }

    const file = fs.openSync(modelPath, "w");
    fs.writeSync(file, meshInfo.buffer);

    for (const buffer of vertexBuffers)
        fs.writeSync(file, buffer);

    for (const buffer of displayListBuffers)
        fs.writeSync(file, buffer);

    for (const material of model.materials) {
        fs.writeSync(file, material.colorBuffer);
    }

    for (const primitiveInfo of primitiveInfos) {
        fs.writeSync(file, primitiveInfo.buffer)
    }

    fs.writeSync(file, vertexPointerCountBuffer);
    for (const buffer of vertexPointerBuffers)
        fs.writeSync(file, buffer);

    if (model.images.length > 0) {
        const textureAssetIndices = prepareTextures(model, outputDir, archive);
        fs.writeSync(file, textureAssetIndices);
    }

    fs.closeSync(file);
}

module.exports = {
    write: write
};
