const processMesh = require("./ProcessMesh");
const processSkinnedMesh = require("./ProcessSkinnedMesh");
const processImage = require("./ProcessImage");
const FontConvert = require("./ProcessFont");
const AudioConvert = require("./AudioConvert");
const processScene = require("./ProcessScene");
const processLevel = require("./ProcessLevel");
const Archive = require("./Archive");

const path = require("path");

async function prepare(manifest, manifestFile, outputDirectory) {
    const manifestDirectory = path.dirname(manifestFile);
    const archive = new Archive();

    if (manifest.meshes) {
        for (const mesh of manifest.meshes) {
            console.log(`Processing Mesh: ${mesh.src}`)

            await processMesh(mesh, archive, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.skinnedMeshes) {
        for (const skinnedMesh of manifest.skinnedMeshes) {
            console.log(`Processing Skinned Mesh: ${skinnedMesh.src}`);
            await processSkinnedMesh(skinnedMesh, archive, manifestDirectory, outputDirectory);
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

            await processImage(manifestDirectory, outputDirectory, image, archive);
        }
    }

    if (manifest.fonts) {
        for (const font of manifest.fonts) {
            if (font.src)
                console.log(`Processing Font: ${font.src}`);
            else
                console.log(`Processing Image Font: ${font.name}`);
            
            await FontConvert.convertFont(manifestDirectory, outputDirectory, font, archive);
        }
    }

    if (manifest.soundBanks) {
        for (const soundBank of manifest.soundBanks) {
            checkRequiredFields("soundBank", soundBank, ["name", "dir"]);

            const sourceDir = path.join(manifestDirectory, soundBank.dir);
            await AudioConvert.convertSoundBank(sourceDir, soundBank.name, outputDirectory, archive);
        }
    }

    if (manifest.musicBanks) {
        for (const musicBank of manifest.musicBanks) {
            checkRequiredFields("musicBank", musicBank, ["name", "dir"]);

            const sourceDir = path.join(manifestDirectory, musicBank.dir);
            await AudioConvert.convertMusicBank(sourceDir, musicBank.name, outputDirectory, archive);
        }
    }

    if (manifest.scenes) {
        const requiredFields = ["src", "typeMap", "layerMap"];

        for (const scene of manifest.scenes) {
            console.log(`Processing Scene: ${scene.src}`);
            checkRequiredFields("scene", scene, requiredFields);

            const typeMap = manifest.typeMaps[scene.typeMap];
            const layerMap = manifest.layerMaps[scene.layerMap];

            await processScene(scene, typeMap, layerMap, archive, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.levels) {
        const requiredFields = ["src", "typeMap", "layerMap"];

        for (const level of manifest.levels) {
            console.log(`Processing Level: ${level.src}`);
            checkRequiredFields("level", level, requiredFields);

            const typeMap = manifest.typeMaps[level.typeMap];
            const layerMap = manifest.layerMaps[level.layerMap];

            await processLevel(level, typeMap, layerMap, archive, manifestDirectory, outputDirectory);
        }
    }

    if (manifest.raw) {
        for (const item of manifest.raw) {
            console.log(`Processing Raw File: ${item}`);
            const sourceFile = path.join(manifestDirectory, item);
            await archive.add(sourceFile, "raw");
        }
    }

    archive.write(outputDirectory);
}

function checkRequiredFields(type, obj, fields) {
    for (const field of fields) {
        if (!obj.hasOwnProperty(field))
            throw new Error(`${type} object must have the following properties: `+ fields.join(' '));
    }
}

module.exports = prepare;