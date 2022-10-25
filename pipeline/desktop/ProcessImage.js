const Image = require("../Image");
const ImageAtlasDefines = require("../ImageAtlasDefines");

const fs = require("fs");
const path = require("path");

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

    const image = new Image();
    await image.loadAtlas(framePaths, imageJson.hslices, imageJson.vslices, parseInt(frameSize[0]), parseInt(frameSize[1]));

    if (imageJson.resize)
        resizeImage(image, imageJson);

    const atlasName = imageJson.name + ".png";
    const outputPath = path.join(outputDirectory, atlasName);

    await image.writeToFile(outputPath);
    const assetId = bundle.addImage(atlasName, imageJson.hslices, imageJson.vslices);

    return {
        assetId: assetId,
        path: outputPath,
        width: image.width,
        height: image.height,
        hslices: imageJson.hslices,
        vslices: imageJson.vslices
    };
}

async function processImage(imageJson, bundle, baseDirectory, outputDirectory) {
    if (imageJson.frames || imageJson.frameDir) {
        ImageAtlasDefines.writeHeaderFile(imageJson, baseDirectory, outputDirectory);
        return buildAtlasFromFrames(imageJson, bundle, baseDirectory, outputDirectory);
    }

    const srcPath = path.join(baseDirectory, imageJson.src);
    const destPath = path.join(outputDirectory, imageJson.src);

    const image = new Image();
    await image.load(srcPath);

    if (imageJson.resize) {
        resizeImage(image, imageJson);
        await image.writeToFile(destPath);
    }
    else { // no operations needed, simply copy to output directory
        fs.copyFileSync(srcPath, destPath);
    }

    const assetId = bundle.addImage(imageJson.src, imageJson.hslices, imageJson.vslices);

    return {
        assetId: assetId,
        width: image.width,
        height: image.height,
        hslices: imageJson.hslices,
        vslices: imageJson.vslices
    };
}

module.exports = processImage;