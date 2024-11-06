const BuildInfo = require("../BuildInfo");
const DesktopAssetBundle = require("./AssetBundle");
const Environment = require("../Environment");
const Util = require("../Util");

const processFont = require("./ProcessFont");
const processImage = require("./ProcessImage");
const processMesh = require("./ProcessMesh");
const processSkinnedMesh = require("./ProcessSkinnedMesh");
const processMusicBank = require("./ProcessMusicBank");
const processSoundBank = require("./ProcessSoundBank");
const processLevel = require("./ProcessLevel")
const processLayers = require("../ProcessLayers");
const processFile = require("./ProcessFile");

const fs = require("fs")
const path = require("path");

async function processDesktop(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const bundle = new DesktopAssetBundle(outputDirectory);
    const layerMap = processLayers(path.dirname(manifestFile), includeDirectory);
    const pipelinePath = path.normalize(path.join(__dirname, ".."));
    const environment = new Environment(BuildInfo.current, bundle, assetDirectory, outputDirectory, includeDirectory, pipelinePath);

    if (manifest.images) {
        for (const image of manifest.images) {
            if (image.src)
                console.log(`Processing Image: ${image.src}`);
            else
                console.log(`Processing Image Atlas: ${image.name}`);

            await processImage(image, bundle, assetDirectory, outputDirectory);
        }
    }

    if (manifest.fonts) {
        for (const font of manifest.fonts) {
            if (font.src)
                console.log(`Processing Font: ${font.src}`);
            else
                console.log(`Processing Image Font: ${font.name}`);

            await processFont(font, bundle, assetDirectory, outputDirectory);
        }
    }

    if (manifest.meshes) {
        for (const mesh of manifest.meshes) {
            console.log(`Processing Mesh: ${mesh.src}`);
            await processMesh(mesh, bundle, assetDirectory, outputDirectory);
        }
    }

    if (manifest.skinnedMeshes) {
        for (const skinnedMesh of manifest.skinnedMeshes) {
            console.log(`Processing Skinned Mesh: ${skinnedMesh.src}`);
            await processSkinnedMesh(skinnedMesh, bundle, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.levels) {
        for (const level of manifest.levels) {
            console.log(`Processing Level: ${level.src}`);

            await processLevel(level, layerMap, bundle, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.soundBanks) {
        for (const soundBank of manifest.soundBanks) {
            console.log(`Processing Sound Bank: ${soundBank.dir}`);
            await processSoundBank(soundBank, bundle, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            console.log(`Processing Music Bank: ${musicBank.dir}`);
            await processMusicBank(musicBank, bundle, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.files) {
        for (const file of manifest.files) {
            console.log(`Processing File: ${file.src}`);
            await processFile(file, environment, pluginMap);
        }
    }

    bundle.writeHeader(path.join(includeDirectory, "assets.h"));
    bundle.writeAssetBundle(path.join(outputDirectory, "asset_bundle.txt"));
    bundle.writeManifest(path.join(outputDirectory, "manifest.txt"))
}

module.exports = processDesktop;