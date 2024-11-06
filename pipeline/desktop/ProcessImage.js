const Image = require("./Image");
const ImageAtlasDefines = require("../ImageAtlasDefines");

const fs = require("fs-extra");
const path = require("path");
const ImageWriter = require("./ImageWriter");
const Util = require("../Util");
const AtlasImage = require("../AtlasImage");

async function processImage(imageJson, bundle, baseDirectory, outputDirectory) {
    if (Object.hasOwn(imageJson, "atlas")) {
        return _processAtlas(imageJson, bundle, baseDirectory, outputDirectory);
    }

    if (imageJson.frames || imageJson.frameDir) {
        ImageAtlasDefines.writeHeaderFile(imageJson, baseDirectory, outputDirectory);
        return buildAtlasFromFrames(imageJson, bundle, baseDirectory, outputDirectory);
    }
    else {
        return _processSprite(imageJson, bundle, baseDirectory, outputDirectory)
    }
}

async function _processAtlas(imageJson, bundle, baseDirectory, outputDirectory) {
    if (!Object.hasOwn(imageJson, "name")) {
        throw new Error("Image atlas must specify a 'name' property.");
    }

    if (Object.hasOwn(imageJson, "additionalPalettes")) {
        throw new Error("Image atlas does not currently support multiple palettes.")
    }

    const atlasImage = await AtlasImage.build(imageJson.atlas, baseDirectory);
    const image = new Image(imageJson.name, atlasImage);

    await _processImage(image, imageJson, bundle, outputDirectory)

    return image;
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

function getFrameArray(imageJson, baseDirectory) {
    if (imageJson.frames) {
        return imageJson.frames;
    }
    else {
        const frameSrcDir = path.join(baseDirectory, imageJson.frameDir);
        const files = fs.readdirSync(frameSrcDir);
        return files.map(file => path.join(imageJson.frameDir, file));
    }
}

// todo: validate sprite params
async function buildAtlasFromFrames(imageJson, bundle, baseDirectory, outputDirectory) {
    if (!imageJson.name) {
        throw new Error("Sprite Atlas elements must specify a name");
    }

    const frames = getFrameArray(imageJson, baseDirectory);
    const frameSize = imageJson.frameSize.split('x');
    const framePaths = frames.map(framePath => path.join(baseDirectory, framePath));

    const image = new Image(Util.safeDefineName(imageJson.name));
    await image.loadAtlas(framePaths, imageJson.hslices, imageJson.vslices, parseInt(frameSize[0]), parseInt(frameSize[1]));
    image.setSliceCounts(imageJson.hslices, imageJson.vslices);
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