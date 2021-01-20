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

    N64ModelWriter.writeHeader(gltfLoader.model, outputFolder);
}

function main() {
    const { program } = require('commander');

    program.version("0.0.1");
    program.requiredOption("-f, --file <path>", "input file");
    program.requiredOption("-o, --out-dir <dir>", "output directory");
    program.option("-m --merge", "attempt to merge meshes when possible", false);

    program.parse(process.argv);

    const gltfPath = program.file;
    const outputFolder = program.outDir;

    const options = {
        mergeMeshes: prgram.merge ? program.merge : false
    }

    if (!fs.existsSync(gltfPath)) {
        console.log(`Path does not exist: ${gltfPath}`)
        process.exit(1)
    }
}

if (require.main === module) {
    main();
}

module.exports = gltfConvert;