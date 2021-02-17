const N64Image = require("./N64Image");
const N64Material = require("./N64Material");
const N64Mesh = require("./N64Mesh");
const N64ImageWriter = require("./N64ImageWriter");
const N64SpriteWriter = require("./N64SpriteWriter");
const DisplayList = require("./DisplayList");
const Bounding = require("./Bounding");

const path  = require("path");
const fs = require("fs")

function writeHeader(model, outputFolder) {
    const filePath = path.join(outputFolder, `${model.name}_model.h`)
    console.log(`write: ${filePath}`);

    const file = fs.openSync(filePath, "w");
    fs.writeSync(file, `#ifndef MODEL_${model.name.toUpperCase()}_H\n`);
    fs.writeSync(file, `#define MODEL_${model.name.toUpperCase()}_H\n\n`);

    for (let i = 0; i < model.images.length; i++) {
        const image = model.images[i];
        const imageName = `model_${model.name}_image_${i}`;

        fs.writeSync(file, `#define ${imageName}_WIDTH ${image.width}\n`);
        fs.writeSync(file, `#define ${imageName}_HEIGHT ${image.height}\n\n`);

        const buffer = N64Image.encode16bpp(image.data, image.width, image.height);
        N64ImageWriter.writeDataArray(file,imageName , buffer, image.width, image.height);
    }

    if (model.hasNormals) {
        for (let i = 0; i < model.materials.length; i++) {
            const material = model.materials[i];
            fs.writeSync(file, `Lights1 ${model.name}_lights_${i} = gdSPDefLights1(${material.ambient[0]}, ${material.ambient[1]}, ${material.ambient[2]}, ${material.color[0]}, ${material.color[1]}, ${material.color[2]}, ${material.lightDirection[0]}, ${material.lightDirection[1]}, ${material.lightDirection[2]});\n\n`);
        }
    }

    fs.writeSync(file, `const float ${model.name}_bounding_box[] = { ${model.bounding.toArrayStr()} };\n\n`);

    for (let i = 0; i < model.meshes.length; i++) {
        const mesh = model.meshes[i];

        const slices = mesh.slice();
        const vertexArrayVar = `${model.name}_mesh_${i}`;

        // Write out mesh vertices for all slices in this mesh
        fs.writeSync(file, `const Vtx ${vertexArrayVar}[] = {\n`);
        for (const slice of slices) {
            for (const v of slice.vertices) {
                fs.writeSync(file, `{ ${v[0]}, ${v[1]}, ${v[2]}, ${v[3]}, ${v[4]}, ${v[5]}, ${v[6]}, ${v[7]}, ${v[8]}, ${v[9]} },\n`)
            }
        }
        fs.writeSync(file, `};\n\n`);

        // write out the mesh display list
        const displayListVar = vertexArrayVar + "_dl"
        fs.writeSync(file, `const Gfx ${displayListVar}[] = {\n`);

        if (mesh.hasNormals) {
            fs.writeSync(file, `gsSPSetLights1(${model.name}_lights_${mesh.material}),\n`);
        }

        let vertexListOffset = 0;

        for (let i = 0; i < slices.length; i++) {
            const slice = slices[i];
            // set the vertex pointer
            fs.writeSync(file, `gsSPVertex(${vertexArrayVar} + ${vertexListOffset}, ${slice.vertices.length}, 0),\n`);

            // write out all the triangles
            for (const t of slice.elements) {
                fs.writeSync(file, `gsSP1Triangle(${t[0]}, ${t[1]}, ${t[2]}, 0),\n`)
            }

            vertexListOffset += slice.vertices.length;
        }

        // end the display list
        fs.writeSync(file, "gsSPEndDisplayList()\n");
        fs.writeSync(file, `};\n\n`);
    }

    fs.writeSync(file, `#endif`);
    fs.closeSync(file);
}

function writeWireframeHeader(model, wireframe, path) {
    const header = fs.openSync(path, "w");

    fs.writeSync(header, `#ifndef ${model.name}_WIRE_MODEL_H\n`);
    fs.writeSync(header, `#define ${model.name}_WIRE_MODEL_H\n\n`);

    fs.writeSync(header, `extern const float ${model.name}_bounding_box[6];\n\n`);

    fs.writeSync(header, "#endif\n");

    fs.closeSync(header);
}

// writes a vertex list for all slices of a mesh
function writeSliceResultVertexArray(file, variableName, slices) {
    fs.writeSync(file, `const Vtx ${variableName}[] = {\n`);
    for (const slice of slices) {
        for (const v of slice.vertices) {
            fs.writeSync(file, `{ ${v[0]}, ${v[1]}, ${v[2]}, ${v[3]}, ${v[4]}, ${v[5]}, ${v[6]}, ${v[7]}, ${v[8]}, ${v[9]} },\n`)
        }
    }
    fs.writeSync(file, `};\n\n`);
}

function writeWireframeSource(model, wireframe, path) {
    const source = fs.openSync(path, "w");

    fs.writeSync(source, `const float ${model.name}_bounding_box[] = { ${model.bounding.toArrayStr()} };\n\n`);

    for (let i  = 0; i < wireframe.length; i++) {
        const sliceResult = wireframe[i];
        const vertexArrayVar = `${model.name}_wire_mesh_${i}_verts`;
        writeSliceResultVertexArray(source, vertexArrayVar, sliceResult);

        const displayListVar = `${model.name}_wire_mesh_${i}_dl`;
        fs.writeSync(source, `const Gfx ${displayListVar}[] = {\n`);

        let vertexListOffset = 0;

        for (let i = 0; i < sliceResult.length; i++) {
            const slice = sliceResult[i];
            // set the vertex pointer
            fs.writeSync(source, `gsSPVertex(${vertexArrayVar} + ${vertexListOffset}, ${slice.vertices.length}, 0),\n`);

            // write out all the triangles
            for (let l = 0; l < slice.lines.length; l+= 2) {
                fs.writeSync(source, `gsSPLine3D(${slice.lines[l]}, ${slice.lines[l + 1]}, 0),\n`);
            }

            vertexListOffset += slice.vertices.length;
        }

        // end the display list
        fs.writeSync(source, "gsSPEndDisplayList()\n");
        fs.writeSync(source, `};\n\n`);
    }


    fs.closeSync(source);
}

function writeWireframe(model, wireframe, outputDir) {
    const filename = model.name + "_wire_model"
    const headerFileName = `${filename}.h`;
    const sourceFileName = `${filename}.c`;

    const headerPath = path.join(outputDir, headerFileName);
    const sourcePath = path.join(outputDir, sourceFileName);

    writeWireframeHeader(model, wireframe, headerPath);
    writeWireframeSource(model, wireframe, sourcePath);
}

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
    writeHeader: writeHeader,
    writeWireframe: writeWireframe,
    write: write
};
