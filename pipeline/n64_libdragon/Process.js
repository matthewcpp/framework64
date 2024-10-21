const Util = require("../Util");

const processFile = require("./ProcessFile");
const processImage = require("./ProcessImage");
const processMesh = require("./ProcessMesh");
const processMusicBank = require("./ProcessMusicBank");
const processSkinnedMesh = require("./ProcessSkinnedMesh");

const DfsAssets = require("./DfsAssets");

const fs = require("fs");
const fse = require("fs-extra");
const path = require("path");

async function processN64Libdragon(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    console.log(`n64_libdragon build assets: ${outputDirectory}`);
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const dfsDirectory = path.join(outputDirectory, "dfs");
    fse.ensureDirSync(dfsDirectory);

    const dfsAssets = new DfsAssets();

    if (manifest.images) {
        for (const image of manifest.images) {
            console.log(`Processing Image: ${image.src}`);
            await processImage(image, dfsAssets, assetDirectory, dfsDirectory, pluginMap);
        }
    }

    if (manifest.meshes) {
        for (const mesh of manifest.meshes) {
            console.log(`Processing Mesh: ${mesh.src}`);
            await processMesh(mesh, dfsAssets, assetDirectory, dfsDirectory, pluginMap);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            console.log(`Processing Music Bank: ${musicBank.dir}`);
            await processMusicBank(musicBank, dfsAssets, assetDirectory, dfsDirectory, includeDirectory);
        }
    }

    if (manifest.skinnedMeshes) {
        for (const skinnedMesh of manifest.skinnedMeshes) {
            console.log(`Processing Skinned Mesh: ${skinnedMesh.src}`);
            await processSkinnedMesh(skinnedMesh, dfsAssets, assetDirectory, dfsDirectory, includeDirectory);
        }
    }

    if (manifest.files) {
        for (const file of manifest.files) {
            console.log(`Processing File: ${file.src}`);
            await processFile(file, dfsAssets, assetDirectory, dfsDirectory, includeDirectory, pluginMap);
        }
    }

    // create the DFS asset bundle and write out the header and manifest
    const headerPath = path.join(Util.assetIncludeDirectory(outputDirectory), "assets.h");
    dfsAssets.writeHeader(headerPath);
    dfsAssets.writeManifest(path.join(outputDirectory, "dfs_manifest.txt"));
    await dfsAssets.makeBundle(dfsDirectory, outputDirectory);
}

module.exports = processN64Libdragon;
