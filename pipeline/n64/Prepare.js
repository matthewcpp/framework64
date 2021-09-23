const gltfConvert = require("./GLTFConvert");
const imageConvert = require("./ImageConvert");
const FontConvert = require("./FontConvert");
const AudioConvert = require("./AudioConvert");
const Archive = require("./Archive");

const path = require("path");

async function prepare(manifest, manifestFile, outputDirectory) {
    const manifestDirectory = path.dirname(manifestFile);
    const archive = new Archive();

    if (manifest.models) {
        for (const model of manifest.models) {
            const sourceFile = path.join(manifestDirectory, model.src);

            await gltfConvert(sourceFile, outputDirectory, model, archive);
        }
    }

    if (manifest.images) {
        for (const image of manifest.images) {
            if (image.src) {
                const sourceFile = path.join(manifestDirectory, image.src);
                await imageConvert.convertSprite(sourceFile, outputDirectory, image, archive);
            }
            else if (image.frames){
                await imageConvert.assembleSprite(manifestDirectory, outputDirectory, image, archive);
            }
            else {
                throw new Error("Image element must specify 'src' or 'frames'");
            }
        }
    }

    if (manifest.fonts) {
        for (const font of manifest.fonts) {
            const sourceFile = path.join(manifestDirectory, font.src);
            await FontConvert.convertFont(sourceFile, outputDirectory, font, archive);
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

        if (manifest.raw) {
            for (const item of manifest.raw) {
                const sourceFile = path.join(manifestDirectory, item);
                archive.add(sourceFile, "raw");
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