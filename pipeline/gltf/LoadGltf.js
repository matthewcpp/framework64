const GLTFLoader = require("./GLTFLoader");

const fse = require("fs-extra");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

/** gltfPath should be the absolute path to the asset to be opened */
async function processGltfSeparate(gltfPath) {
    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadFile(gltfPath);

    return gltfLoader;
}

const MacOsBlenderSearchPaths = ["/Applications/Blender.app/Contents/MacOS"]

async function getDefaultPlatformBlenderSearchPaths() {
    switch(process.platform) {
        case "darwin":
            return MacOsBlenderSearchPaths;
        
        default:
            return [];
    }
}

async function processBlenderFile(assetJson, environment) {
    const source = path.join(environment.assetDirectory, assetJson.src);
    let blenderPath = null;

    const defaultSearchPaths = getDefaultPlatformBlenderSearchPaths();
    for (const defaultSearchPath of defaultSearchPaths) {
        const testPath = path.join(defaultSearchPath, "blender");
        if (fse.existsSync(testPath)) {
            blenderPath = testPath;
            break;
        }
    }

    if (blenderPath === null) {
        throw new Error(`Unable to process ${source}: could not determine path to blender executable`);
    }

    // Determine a directory and file to export the GLTF from blender into
    const gltfName =  Object.hasOwn(assetJson, "name") ? assetJson.name : path.basename(assetJson.src, path.extname(assetJson.src));
    const blenderOuputDirectory = path.join(environment.outputDirectory, "_blenderGltf", gltfName);
    const blenderOutputGltfFile = path.join(blenderOuputDirectory, gltfName + ".gltf");
    console.log(`exporting gltf to: ${blenderOuputDirectory}`);
    fse.ensureDirSync(blenderOuputDirectory);

    const pythonBlenderExportScript = path.join(__dirname, "ExportGltf.py");
    if (!fse.existsSync(pythonBlenderExportScript)) {
        throw new Error(`Unable to process ${source}: could not locate ExportGltf.py.  Expected: ${pythonBlenderExportScript}`)
    }

    const runPipelineArgs = [
        assetJson.src,
        "--background",
        "--python", pythonBlenderExportScript,
        "--",
        blenderOutputGltfFile,
        "--animationSampleStep", "4"
    ];

    await execFile(blenderPath, runPipelineArgs);
    return await processGltfSeparate(blenderOutputGltfFile);
}

async function loadGltf(assetJson, environment) {
    const extension = path.extname(assetJson.src);

    switch (extension) {
        case ".gltf":
            return await processGltfSeparate(path.join(environment.assetDirectory, assetJson.src));
        
        case ".blend":
            return await processBlenderFile(assetJson, environment);

        default:
            throw new Error("not implemented yet.")
    }
}

module.exports = loadGltf;