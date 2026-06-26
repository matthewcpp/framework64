const Image = require("./Image")
const ImageWriter = require("./ImageWriter");
const ImageAtlasDefines = require("../ImageAtlasDefines");
const ProcessImageUtils = require("../ProcessImageUtils");

const Jimp = require("jimp");

const fs = require("fs");
const path = require("path");

async function processImage(imageJson, archive, manifestDirectory, outputDirectory) {
    if (Object.hasOwn(imageJson, "src")) {
        return convertSprite(manifestDirectory, outputDirectory, imageJson, archive);
    }
    else if (Object.hasOwn(imageJson, "frames") || Object.hasOwn(imageJson, "frameDir")){
        return assembleSpriteAtlas(imageJson, manifestDirectory, outputDirectory, archive);
    }
}

async function convertSprite(manifestDirectory, outDir, imageJson, archive) {
    const imagePath = path.join(manifestDirectory, imageJson.src);

    const options = {
        hslices: 1,
        vslices: 1,
        format: "RGBA16"
    }
    Object.assign(options, imageJson);

    const name = Object.hasOwn(imageJson, "name") ? imageJson.name : path.basename(imagePath, path.extname(imagePath));

    const image = new Image(name, Image.Format[options.format.toUpperCase()]);
    await image.load(imagePath);

    return finalizeImage(image, manifestDirectory, outDir, options, archive, imageJson.name)
}

async function assembleSpriteAtlas(imageJson, rootDir, outDir, archive) {
    if (!Object.hasOwn(imageJson, "name")) {
        throw new Error("Sprite Atlas elements must specify a name");
    }

    const options = {
        format: "RGBA16"
    }

    Object.assign(options, imageJson);

    const image = new Image(imageJson.name, Image.Format[options.format.toUpperCase()]);

    const framePaths = ProcessImageUtils.getFramePathArray(imageJson, rootDir);
    const frameSize = imageJson.frameSize.split('x');
    await image.loadAtlas(framePaths, imageJson.hslices, imageJson.vslices, parseInt(frameSize[0]), parseInt(frameSize[1]));

    // If we do not have outdir then we are writing an internal image and these defines would not be needed
    // This will be the case when writing materials for meshes
    if (!!outDir){
        ImageAtlasDefines.writeHeaderFile(imageJson, rootDir, outDir);
    }

    return finalizeImage(image, rootDir, outDir, imageJson, archive, imageJson.name)
}

async function finalizeImage(image, assetDir, outDir, imageJson, archive) {
    if (imageJson.resize) {
        const dimensions = imageJson.resize.split("x");
        image.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
    }

    if (image.format === Image.Format.CI8 || image.format === Image.Format.CI4) {
        image.createColorIndexImage();

        if (imageJson.additionalPalettes) {
            for (const paletteFile of imageJson.additionalPalettes) {
                const paletteFilePath = path.join(assetDir, paletteFile);
                await image.colorIndexImage.addPaletteFromPath(paletteFilePath);
            }
        }
    }

    let assetIndex = -1;
    let assetBuffer = null;

    if (archive) {
        const filePath = path.join(outDir, `${image.name}.image`);
        ImageWriter.writeFile(image, imageJson.hslices, imageJson.vslices, filePath);
        assetIndex = archive.addImage(filePath, image.name);
    }
    else {
        assetBuffer = ImageWriter.writeBuffer(image, imageJson.hslices, imageJson.vslices);
    }

    return {
        name: image.name,
        assetIndex: assetIndex,
        assetBuffer: assetBuffer,
        width: image.width,
        height: image.height,
        hslices: imageJson.hslices,
        vslices: imageJson.vslices,
    };
}

module.exports = processImage;