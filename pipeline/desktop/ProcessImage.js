const Image = require("./Image");
const ImageAtlasDefines = require("../ImageAtlasDefines");
const ProcessImageUtils = require("../ProcessImageUtils");

const fs = require("fs-extra");
const path = require("path");
const ImageWriter = require("./ImageWriter");
const Util = require("../Util");

async function processImage(imageJson, bundle, baseDirectory, outputDirectory) {
    if (imageJson.frames || imageJson.frameDir) {
        ImageAtlasDefines.writeHeaderFile(imageJson, baseDirectory, outputDirectory);
        return buildAtlasFromFrames(imageJson, bundle, baseDirectory, outputDirectory);
    }
    else {
        return _processSprite(imageJson, bundle, baseDirectory, outputDirectory)
    }
}

async function _processSprite(imageJson, bundle, baseDirectory, outputDirectory) {
    const srcPath = path.join(baseDirectory, imageJson.src);
    const imageName = (!!imageJson.name) ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src));
    const image = new Image(Util.safeDefineName(imageName));
    await image.load(srcPath);

    const indexMode = imageIndexModes.has(imageJson.format);
    image.isIndexed = indexMode;

    if (indexMode && (!!imageJson.additionalPalettes)) {
        for (const palettePath of imageJson.additionalPalettes) {

            const paletteImageSrcPath = path.join(baseDirectory, palettePath);
            image.additionalPalettes.push(fs.readFileSync(paletteImageSrcPath));
        }
    }

    await _processImage(image, imageJson, bundle, outputDirectory)

    return image;
}

function resizeImage(image, sprite) {
    const dimensions = sprite.resize.split("x");

    const width = parseInt(dimensions[0]);
    const height = parseInt(dimensions[1]);

    image.resize(width, height);
}

// todo: validate sprite params
async function buildAtlasFromFrames(imageJson, bundle, baseDirectory, outputDirectory) {
    if (!Object.hasOwn(imageJson, "name")) {
        throw new Error("Sprite Atlas elements must specify a name");
    }

    const image = new Image(Util.safeDefineName(imageJson.name));
    
    const framePaths = ProcessImageUtils.getFramePathArray(imageJson, baseDirectory);
    const frameSize = imageJson.frameSize.split('x');
    await image.loadAtlas(framePaths, imageJson.hslices, imageJson.vslices, parseInt(frameSize[0]), parseInt(frameSize[1]));
    
    image.isIndexed = imageIndexModes.has(imageJson.format);

    if (imageJson.resize)
        resizeImage(image, imageJson);

    if (bundle) {
        const imageFileName = image.name + ".image";
        const destPath = path.join(outputDirectory, imageFileName);
        await ImageWriter.writeFile(image, destPath);
        bundle.addImage(imageFileName, image.name);
    }

    return image;
}

const imageIndexModes = new Set(["CI8", "CI4"]);

async function _processImage(image, imageJson, bundle, outputDirectory) {
    const hslices = Object.hasOwn(imageJson, "hslices") ? imageJson.hslices : 1;
    const vslices = Object.hasOwn(imageJson, "vslices") ? imageJson.vslices : 1;
    image.setSliceCounts(hslices, vslices);
    
    if (Object.hasOwn(imageJson, "resize")) {
        resizeImage(image, imageJson);
    }

    if (bundle) {
        const imageFileName = image.name + ".image";
        const destPath = path.join(outputDirectory, imageFileName);
        fs.ensureDirSync(path.dirname(destPath));

        await ImageWriter.writeFile(image, destPath);
        bundle.addImage(imageFileName, image.name);
    }
}

module.exports = processImage;