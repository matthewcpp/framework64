const N64ImageWriter = require("./N64ImageWriter");

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
        N64ImageWriter.writeDataArray(file,imageName , image.data, image.width, image.height);
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
        //console.log(`Mesh: ${model.meshes[i].vertices.length} vertices, ${model.meshes[i].triangles.length} triangles`);

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
            for (const t of slice.triangles) {
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

module.exports = {
    writeHeader: writeHeader,
    writeWireframe: writeWireframe
};
