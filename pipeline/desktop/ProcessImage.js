const Image = require("../Image");

const fs = require("fs");
const path = require("path");

function resizeImage(image, sprite) {
    const dimensions = sprite.resize.split("x");

    const width = parseInt(dimensions[0]);
    const height = parseInt(dimensions[1]);

    image.resize(width, height);
}

// todo: validate sprite params
async function buildAtlasFromFrames(imageJson, bundle, baseDirectory, outputDirectory) {
    const frameSize = imageJson.frameSize.split('x');
    const framePaths = imageJson.frames.map(framePath => path.join(baseDirectory, framePath));

    const image = new Image();
    await image.loadAtlas(framePaths, imageJson.hslices, imageJson.vslices, parseInt(frameSize[0]), parseInt(frameSize[1]));

    if (imageJson.resize)
        resizeImage(image, imageJson);

    const atlasName = imageJson.name + ".png";
    const outputPath = path.join(outputDirectory, atlasName);

    await image.writeToFile(outputPath);
    bundle.addImage(atlasName, imageJson.hslices, imageJson.vslices);
}

async function processImage(imageJson, bundle, baseDirectory, outputDirectory) {
    if (imageJson.frames) {
        await buildAtlasFromFrames(imageJson, bundle, baseDirectory, outputDirectory);
        return;
    }

    const srcPath = path.join(baseDirectory, imageJson.src);
    const destPath = path.join(outputDirectory, imageJson.src);

    if (imageJson.resize) {
        const image = new Image();
        await image.load(srcPath);
        resizeImage(image, imageJson);
        await image.writeToFile(destPath);
    }
    else { // no operations needed, simply copy to output directory
        fs.copyFileSync(srcPath, destPath);
    }

    bundle.addImage(imageJson.src, imageJson.hslices, imageJson.vslices);
}

module.exports = processImage;