const Bundle = require("./Bundle");

const processSprite = require("./ProcessImage");
const processFont = require("./ProcessFont");
const processMesh = require("./ProcessMesh");
const processSoundBank = require("./ProcessSoundBank");
const processMusicBank = require("./ProcessMusicBank");
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

            await processSprite(image, bundle, manifestDirectory, outputDirectory);
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

    bundle.finalize();
}

module.exports = prepare;