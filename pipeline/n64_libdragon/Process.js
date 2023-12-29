const Util = require("../Util");

const AssetBundle = require("../AssetBundle");

const processImage = require("./ProcessImage");
const processMesh = require("./ProcessMesh");

const fs = require("fs");
const path = require("path");

async function processN64Libdragon(manifest, assetDirectory, outputDirectory, plugins) {
    const assetBundle = new AssetBundle();

    console.log(`n64_libdragon build assets: ${outputDirectory}`);

    if (manifest.images) {
        for (const imgae of manifest.images) {
            console.log(`Processing Image: ${imgae.src}`);
            await processImage(imgae, assetBundle, assetDirectory, outputDirectory);
        }
    }

    if (manifest.meshes) {
        for (const mesh of manifest.meshes) {
            console.log(`Processing Mesh: ${mesh.src}`);
            await processMesh(mesh, assetBundle, assetDirectory, outputDirectory);
        }
    }

    const dfsDirPath = path.join(outputDirectory, "dfs");
    if (!fs.existsSync(dfsDirPath)) {
        fs.mkdirSync(dfsDirPath);
    }

    const bundlePath = path.join(dfsDirPath, "assets.data");
    const headerPath = path.join(Util.assetIncludeDirectory(outputDirectory), "assets.h");
    assetBundle.write(bundlePath, headerPath);
}

module.exports = processN64Libdragon;
