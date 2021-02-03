const GLTFLoader = require("./GLTFLoader");
const N64ModelWriter = require("./N64ModelWriter")

const fs = require("fs");
const path = require("path");

async function gltfConvert(gltfPath, outputFolder, params) {
    const gltfLoader = new GLTFLoader(params);
    await gltfLoader.load(gltfPath);

    if (params.hasOwnProperty("mergeMeshes") && params.mergeMeshes) {
        gltfLoader.merge();
    }

    if (params.createWireframe) {
        let wireColor = null;

        if (params.wireColor) {
            const values = params.wireColor.split(',');
            wireColor = [parseInt(values[0]), parseInt(values[1]), parseInt(values[2])];
        }

        const wireframe = gltfLoader.model.createWireframe(wireColor);
        N64ModelWriter.writeWireframe(gltfLoader.model, wireframe, outputFolder);
    }
    else {
        N64ModelWriter.writeHeader(gltfLoader.model, outputFolder);
    }
}

async function main() {
    const { program } = require('commander');

    program.version("0.0.1");
    program.requiredOption("-f, --file <path>", "input file");
    program.requiredOption("-o, --out-dir <dir>", "output directory");
    program.option("-m --merge", "attempt to merge meshes when possible", false);
    program.option("--wire", "create a simple wireframe representation of the model", false);
    program.option("--wire-color <r,g,b>", "specify a override color for wireframe.  Otherwise default material color is used.")

    program.parse(process.argv);

    const gltfPath = program.file;
    const outputFolder = program.outDir;

    const options = {
        mergeMeshes: program.merge,
        createWireframe: program.wire
    }

    if (!fs.existsSync(gltfPath)) {
        console.log(`Source file does not exist: ${gltfPath}`)
        process.exit(1)
    }

    if (!fs.existsSync(outputFolder)) {
        console.log(`Output directory does not exist: ${gltfPath}`)
        process.exit(1)
    }

    await gltfConvert(gltfPath, outputFolder, options);
}

if (require.main === module) {
    main();
}

module.exports = gltfConvert;