const Bundle = require("./Bundle");
const Util = require("../Util");

const processFont = require("./ProcessFont");
const processImage = require("./ProcessImage");
const processMesh = require("./ProcessMesh");
const processSkinnedMesh = require("./ProcessSkinnedMesh");
const processMusicBank = require("./ProcessMusicBank");
const processSoundBank = require("./ProcessSoundBank");
const processLevel = require("./ProcessLevel")
const processLayers = require("../Layers");

const processRaw = require("./ProcessRaw");

const path = require("path");

async function prepare(manifest, assetDirectory, outputDirectory, plugins) {
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const bundle = new Bundle(outputDirectory, includeDirectory);
    const layerMap = processLayers(manifest.layers, includeDirectory);

    plugins.initialize(bundle, assetDirectory, outputDirectory, includeDirectory, "desktop");

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
            await processMesh(mesh, bundle, assetDirectory, outputDirectory, plugins);
        }
    }

    if (manifest.skinnedMeshes) {
        for (const skinnedMesh of manifest.skinnedMeshes) {
            console.log(`Processing Skinned Mesh: ${skinnedMesh.src}`);
            await processSkinnedMesh(skinnedMesh, bundle, assetDirectory, outputDirectory, includeDirectory, plugins);
        }
    }

    if (manifest.levels) {
        for (const level of manifest.levels) {
            console.log(`Processing Level: ${level.src}`);

            await processLevel(level, layerMap, bundle, assetDirectory, outputDirectory, includeDirectory, plugins);
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

/*
    if (manifest.raw) {
        for (const rawFile of manifest.raw) {
            console.log(`Processing Raw File: ${rawFile}`);
            processRaw(rawFile, bundle, assetDirectory, outputDirectory);
        }
    }
*/

    bundle.finalize();
}

module.exports = prepare;