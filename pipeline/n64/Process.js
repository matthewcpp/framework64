const processMesh = require("./ProcessMesh");
const processSkinnedMesh = require("./ProcessSkinnedMesh");
const processImage = require("./ProcessImage");
const FontConvert = require("./ProcessFont");
const AudioConvert = require("./AudioConvert");
const processLevel = require("./ProcessLevel");
const processLayers = require("../ProcessLayers");
const Archive = require("./Archive");
const Util = require("../Util");

const path = require("path");

async function processN64(manifest, assetDirectory, outputDirectory, pluginMap) {
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const archive = new Archive(outputDirectory, includeDirectory);

    const layerMap = processLayers(assetDirectory, Util.assetIncludeDirectory(outputDirectory));

    if (manifest.meshes) {
        for (const mesh of manifest.meshes) {
            console.log(`Processing Mesh: ${mesh.src}`)

            await processMesh(mesh, archive, assetDirectory, outputDirectory);
        }
    }

    if (manifest.skinnedMeshes) {
        for (const skinnedMesh of manifest.skinnedMeshes) {
            console.log(`Processing Skinned Mesh: ${skinnedMesh.src}`);
            await processSkinnedMesh(skinnedMesh, archive, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.images) {
        for (const image of manifest.images) {
            if (image.src) {
                console.log(`Processing Image: ${image.src}`);
            }
            else if (image.frames || image.frameDir){
                console.log(`Processing Image Atlas: ${image.name}`);
            }

            await processImage(image, archive, assetDirectory, outputDirectory);
        }
    }

    if (manifest.fonts) {
        for (const font of manifest.fonts) {
            if (font.src)
                console.log(`Processing Font: ${font.src}`);
            else
                console.log(`Processing Image Font: ${font.name}`);
            
            await FontConvert.convertFont(assetDirectory, outputDirectory, font, archive);
        }
    }

    if (manifest.levels) {
        const requiredFields = ["src"];

        for (const level of manifest.levels) {
            console.log(`Processing Level: ${level.src}`);
            checkRequiredFields("level", level, requiredFields);

            await processLevel(level, layerMap, archive, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.raw) {
        for (const item of manifest.raw) {
            console.log(`Processing Raw File: ${item}`);
            const sourceFile = path.join(assetDirectory, item);
            await archive.add(sourceFile, "raw");
        }
    }

    if (manifest.soundBanks) {
        for (const soundBank of manifest.soundBanks) {
            checkRequiredFields("soundBank", soundBank, ["name", "dir"]);

            const sourceDir = path.join(assetDirectory, soundBank.dir);
            await AudioConvert.convertSoundBank(sourceDir, soundBank.name, outputDirectory, includeDirectory, archive);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            checkRequiredFields("musicBank", musicBank, ["name", "dir"]);

            const sourceDir = path.join(assetDirectory, musicBank.dir);
            await AudioConvert.convertMusicBank(sourceDir, musicBank.name, outputDirectory, includeDirectory, archive);
        }
    }

    archive.write();
}

function checkRequiredFields(type, obj, fields) {
    for (const field of fields) {
        if (!obj.hasOwnProperty(field))
            throw new Error(`${type} object must have the following properties: `+ fields.join(' '));
    }
}

module.exports = processN64;