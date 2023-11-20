const N64Image = require("./N64Image")
const N64ImageWriter = require("./N64ImageWriter");
const ImageAtlasDefines = require("../ImageAtlasDefines");

const Jimp = require("jimp");

const fs = require("fs");
const path = require("path");

async function processImage(imageJson, archive, manifestDirectory, outputDirectory) {
    if (imageJson.src) {
        return convertSprite(manifestDirectory, outputDirectory, imageJson, archive);
    }
    else if (imageJson.frames || imageJson.frameDir){
        return assembleSpriteAtlas(manifestDirectory, outputDirectory, imageJson, archive);
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

    const name = !!imageJson.name ? imageJson.name : path.basename(imagePath, path.extname(imagePath));

    const image = new N64Image(name, N64Image.Format[options.format.toUpperCase()]);
    await image.load(imagePath);

    return finalizeImage(image, manifestDirectory, outDir, options, archive, imageJson.name)
}

async function assembleSpriteAtlas(rootDir, outDir, imageJson, archive) {
    const options = {
        format: "RGBA16"
    }
    Object.assign(options, imageJson);

    const image = new N64Image(imageJson.name, N64Image.Format[options.format.toUpperCase()]);

    const frames = getFrameArray(imageJson, rootDir);
    const atlas = await buildSpriteAtlas(frames, rootDir, imageJson);
    await image.assign(atlas);

    // if we do not have outdir then we are writing an internal image and these defines would not be needed
    if (!!outDir)
        ImageAtlasDefines.writeHeaderFile(imageJson, rootDir, outDir);

    return finalizeImage(image, rootDir, outDir, imageJson, archive, imageJson.name)
}

function getDimension(imageJson) {
    const frameSize = imageJson.frameSize.split('x');
    return [parseInt(frameSize[0]), parseInt(frameSize[1])];
}

function createNewImage(width, height) {
    return new Promise((resolve) => {
        new Jimp(width, height, (err, img) => {
            resolve(img);
        })
    });
}

async function buildSpriteAtlas(frames, rootDir, imagejson) {
    const [frameWidth, frameHeight] = getDimension(imagejson);

    const atlas = await createNewImage(imagejson.hslices * frameWidth, imagejson.vslices * frameHeight);

    for (let y = 0; y < imagejson.vslices; y++) {
        for (let x = 0; x < imagejson.hslices; x++) {
            const framePath = path.join(rootDir, frames[y * imagejson.hslices + x]);
            const frame = await Jimp.read(framePath);

            atlas.blit(frame, x * frameWidth, y * frameHeight);
        }
    }

    return atlas
}

function getFrameArray(imageJson, rootDir) {
    if (imageJson.frames) {
        return imageJson.frames;
    }
    else {
        const frameSrcDir = path.join(rootDir, imageJson.frameDir);
        const files = fs.readdirSync(frameSrcDir);
        return files.map(file => path.join(imageJson.frameDir, file));
    }
}

async function finalizeImage(image, assetDir, outDir, imageJson, archive) {
    if (imageJson.resize) {
        const dimensions = imageJson.resize.split("x");
        image.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
    }

    if (image.format === N64Image.Format.CI8 || image.format === N64Image.Format.CI4) {
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
        N64ImageWriter.writeFile(image, imageJson.hslices, imageJson.vslices, filePath);
        const entry = await archive.add(filePath, "image");
        assetIndex = entry.index;
    }
    else {
        assetBuffer = N64ImageWriter.writeBuffer(image, imageJson.hslices, imageJson.vslices);
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