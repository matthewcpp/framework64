const Bundle = require("./Bundle");
const Util = require("../Util");
const Environment = require("../Environment");

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
    const bundle = new Bundle(outputDirectory, includeDirectory);
    const layerMap = processLayers(path.dirname(manifestFile), includeDirectory);
    const environment = new Environment();

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
            console.log(`Processing Sound Bank: ${soundBank.name}`);
            await processSoundBank(soundBank, bundle, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            console.log(`Processing Music Bank: ${musicBank.name}`);
            await processMusicBank(musicBank, bundle, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.files) {
        for (const file of manifest.files) {
            console.log(`Processing File: ${file.src}`);
            await processFile(file, bundle, assetDirectory, outputDirectory, includeDirectory, pluginMap, environment);
        }
    }

    bundle.finalize();
}

module.exports = processDesktop;