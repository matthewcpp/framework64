const Util = require("../Util");

const processImage = require("./ProcessImage");
const processMesh = require("./ProcessMesh");

const DfsAssets = require("./DfsAssets");

const fs = require("fs");
const fse = require("fs-extra");
const path = require("path");

async function processN64Libdragon(manifestFile, assetDirectory, outputDirectory, plugins) {
    console.log(`n64_libdragon build assets: ${outputDirectory}`);
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const dfsDirectory = path.join(outputDirectory, "dfs");
    fse.ensureDirSync(dfsDirectory);

    const dfsAssets = new DfsAssets();

    if (manifest.images) {
        for (const image of manifest.images) {
            console.log(`Processing Image: ${image.src}`);
            await processImage(image, dfsAssets, assetDirectory, dfsDirectory);
        }
    }

    const headerPath = path.join(Util.assetIncludeDirectory(outputDirectory), "assets.h");
    dfsAssets.writeHeader(headerPath);
    await dfsAssets.makeBundle(dfsDirectory, outputDirectory);

    // if (manifest.meshes) {
    //     for (const mesh of manifest.meshes) {
    //         console.log(`Processing Mesh: ${mesh.src}`);
    //         await processMesh(mesh, assetBundle, assetDirectory, outputDirectory);
    //     }
    // }

    // const dfsDirPath = path.join(outputDirectory, "dfs");
    // if (!fs.existsSync(dfsDirPath)) {
    //     fs.mkdirSync(dfsDirPath);
    // }

    // const bundlePath = path.join(dfsDirPath, "assets.data");
    // const headerPath = path.join(Util.assetIncludeDirectory(outputDirectory), "assets.h");
    // assetBundle.write(bundlePath, headerPath);
}

module.exports = processN64Libdragon;
