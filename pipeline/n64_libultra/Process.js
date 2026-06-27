const Environment = require("../Environment");
const N64LibUltraAssetBundle = require("./AssetBundle");

const processMesh = require("../MeshProcessor");
const processSkinnedMesh = require("../SkinnedMeshProcessor");
const processImage = require("./ImageProcessor");
const processFile = require("../FileProcessor");
const processFont = require("./FontProcessor");
const processLevel = require("../LevelProcessor");
const processLayers = require("../LayerProcessor");
const processMusicBank = require("./MusicBankProcessor");
const processSoundBank = require("./SoundBankProcessor");
const Util = require("../Util");

const fs = require("fs")
const path = require("path");


async function processN64(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const archive = new N64LibUltraAssetBundle(outputDirectory);
    const pipelinePath = path.normalize(path.join(__dirname, ".."));
    const environment = new Environment("n64_libultra", Environment.Architecture.Arch32, Environment.Endian.Big, archive, 
        manifestFile, assetDirectory, outputDirectory, includeDirectory, pipelinePath);

    const layerMap = processLayers(path.dirname(manifestFile), Util.assetIncludeDirectory(outputDirectory));

    if (manifest.meshes) {
        const MeshWriter = require("./MeshWriter");

        for (const mesh of manifest.meshes) {
            console.log(`Processing Mesh: ${mesh.src}`)
            await processMesh(environment, mesh, MeshWriter);
        }
    }

    if (manifest.skinnedMeshes) {
        const MeshWriter = require("./MeshWriter");

        for (const skinnedMesh of manifest.skinnedMeshes) {
            console.log(`Processing Skinned Mesh: ${skinnedMesh.src}`);
            await processSkinnedMesh(environment, skinnedMesh, MeshWriter);
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
            if (font.src) {
                console.log(`Processing Font: ${font.src}`);
            }
            else{
                console.log(`Processing Image Font: ${font.name}`);
            }
            
            await processFont(assetDirectory, outputDirectory, font, archive);
        }
    }

    if (manifest.levels) {
        const requiredFields = ["src"];
        const materialBundleWriter = require("./MaterialBundleWriter");
        const meshWriter = require("./MeshWriter");

        for (const level of manifest.levels) {
            console.log(`Processing Level: ${level.src}`);
            checkRequiredFields("level", level, requiredFields);

            await processLevel(environment, level, layerMap, materialBundleWriter, meshWriter);
        }
    }

    if (manifest.files) {
        for (const file of manifest.files) {
            console.log(`Processing File: ${file.src}`);
            await processFile(environment, file, pluginMap);
        }
    }

    if (manifest.soundBanks) {
        for (const soundBank of manifest.soundBanks) {
            checkRequiredFields("soundBank", soundBank, ["name", "dir"]);
            console.log(`Processing Sound Bank: ${soundBank.dir}`);

            await processSoundBank(environment, soundBank);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            checkRequiredFields("musicBank", musicBank, ["name", "dir"]);
            console.log(`Processing Music Bank: ${musicBank.dir}`);

            await processMusicBank(environment, musicBank);
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