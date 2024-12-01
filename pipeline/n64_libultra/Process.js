const AudioConvert = require("./AudioConvert");
const Environment = require("../Environment");
const FontConvert = require("./ProcessFont");
const N64LibUltraAssetBundle = require("./AssetBundle");

const processMesh = require("./ProcessMesh");
const processSkinnedMesh = require("./ProcessSkinnedMesh");
const processImage = require("./ProcessImage");
const processFile = require("./ProcessFile");
const processLevel = require("./ProcessLevel");
const processLayers = require("../ProcessLayers");
const Util = require("../Util");

const fs = require("fs")
const path = require("path");


async function processN64(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const archive = new N64LibUltraAssetBundle();
    const pipelinePath = path.normalize(path.join(__dirname, ".."));
    const environment = new Environment("n64_libultra", Environment.Architecture.Arch32, Environment.Endian.Big, archive, assetDirectory, outputDirectory, includeDirectory, pipelinePath);

    const layerMap = processLayers(path.dirname(manifestFile), Util.assetIncludeDirectory(outputDirectory));

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

            await processLevel(environment, level, layerMap, archive, assetDirectory, outputDirectory, includeDirectory);
        }
    }

    if (manifest.files) {
        for (const file of manifest.files) {
            console.log(`Processing File: ${file.src}`);
            await processFile(file, environment, pluginMap);
        }
    }

    if (manifest.soundBanks) {
        for (const soundBank of manifest.soundBanks) {
            checkRequiredFields("soundBank", soundBank, ["name", "dir"]);
            console.log(`Processing Sound Bank: ${soundBank.dir}`);

            const sourceDir = path.join(assetDirectory, soundBank.dir);
            await AudioConvert.convertSoundBank(sourceDir, soundBank.name, outputDirectory, includeDirectory, archive);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            checkRequiredFields("musicBank", musicBank, ["name", "dir"]);
            console.log(`Processing Music Bank: ${musicBank.dir}`);

            const sourceDir = path.join(assetDirectory, musicBank.dir);
            await AudioConvert.convertMusicBank(sourceDir, musicBank.name, outputDirectory, includeDirectory, archive);
        }
    }

    archive.writeHeader(path.join(includeDirectory, "assets.h"));
    archive.writeArchive(path.join(outputDirectory, "assets.dat"));
    archive.writeManifest(path.join(outputDirectory, "manifest.txt"))
}

function checkRequiredFields(type, obj, fields) {
    for (const field of fields) {
        if (!obj.hasOwnProperty(field))
            throw new Error(`${type} object must have the following properties: `+ fields.join(' '));
    }
}

module.exports = processN64;