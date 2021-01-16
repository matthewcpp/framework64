const GLTFLoader = require("./GLTFLoader");
const N64ModelWriter = require("./N64ModelWriter")

const fs = require("fs");
const path = require("path");

function gltfConvert(gltfPath, outputFolder, params) {
    let options = {
        mergeMeshes: false,
        globalScale: 1.0,
        bakeTransform: false
    };

    if (params) {
        Object.assign(options, params);
    }

    const gltfLoader = new GLTFLoader();
    gltfLoader.globalScale = options.globalScale;
    gltfLoader.bakeTransform = options.bakeTransform;
    gltfLoader.load(gltfPath);

    if (options.merge) {
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
    program.option("-s --scale <value>", "scale value to apply to all vertices", "1.0");

    program.parse(process.argv);

    const gltfPath = program.file;
    const outputFolder = program.outDir;

    const options = {
        mergeMeshes: prgram.merge ? program.merge : false,
        globalScale: parseFloat(program.scale)
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