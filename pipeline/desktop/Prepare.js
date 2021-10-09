const Bundle = require("./Bundle");

const processFont = require("./ProcessFont");
const processImage = require("./ProcessImage");
const processMesh = require("./ProcessMesh");
const processMusicBank = require("./ProcessMusicBank");
const processSoundBank = require("./ProcessSoundBank");
const processTerrain = require("./ProcessTerrain")

const processRaw = require("./ProcessRaw");

const path = require("path");

async function prepare(manifest, manifestFile, outputDirectory, filters) {
    const manifestDirectory = path.dirname(manifestFile);

    const bundle = new Bundle(outputDirectory);

    if (manifest.images) {
        for (const image of manifest.images) {
            if (image.src)
                console.log(`Processing Image: ${image.src}`);
            else
                console.log(`Processing Image Atlas: ${image.name}`);

            await processImage(image, bundle, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.fonts) {
        for (const font of manifest.fonts) {
            console.log(`Processing Font: ${font.src}`);
            await processFont(font, bundle, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.meshes) {
        for (const mesh of manifest.meshes) {
            console.log(`Processing Mesh: ${mesh.src}`);
            await processMesh(mesh, bundle, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.soundBanks) {
        for (const soundBank of manifest.soundBanks) {
            console.log(`Processing Sound Bank: ${soundBank.name}`);
            await processSoundBank(soundBank, bundle, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            console.log(`Processing Music Bank: ${musicBank.name}`);
            await processMusicBank(musicBank, bundle, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.raw) {
        for (const rawFile of manifest.raw) {
            console.log(`Processing Raw File: ${rawFile}`);
            processRaw(rawFile, bundle, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.terrains) {
        for (const terrain of manifest.terrains) {
            console.log(`Processing Terrain: ${terrain.src}`);
            await processTerrain(terrain, bundle, manifestDirectory, outputDirectory);
        }
    }

    bundle.finalize();
}

module.exports = prepare;