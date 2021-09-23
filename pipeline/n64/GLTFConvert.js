const GLTFLoader = require("./GLTFLoader");
const N64ModelWriter = require("./N64ModelWriter")

const fs = require("fs");
const path = require("path");

async function gltfConvert(gltfPath, outputFolder, params, archive) {
    const gltfLoader = new GLTFLoader(params);
    await gltfLoader.load(gltfPath);

    N64ModelWriter.write(gltfLoader.model, outputFolder, archive);
}

async function main() {
    const { program } = require('commander');

    program.version("0.1.0");
    program.requiredOption("-f, --file <path>", "input file");
    program.requiredOption("-o, --out-dir <dir>", "output directory");

    program.parse(process.argv);

    const gltfPath = program.file;
    const outputFolder = program.outDir;

    const options = {};

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