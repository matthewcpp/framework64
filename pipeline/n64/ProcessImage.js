const N64Image = require("./N64Image")
const N64ImageWriter = require("./N64ImageWriter");
const ImageAtlasDefines = require("../ImageAtlasDefines");

const Jimp = require("jimp");

const fs = require("fs");
const path = require("path");

async function processImage(manifestDirectory, outputDirectory, image, archive) {
    if (image.src) {
        return convertSprite(manifestDirectory, outputDirectory, image, archive);
    }
    else if (image.frames || image.frameDir){
        ImageAtlasDefines.writeHeaderFile(image, manifestDirectory, outputDirectory);
        return assembleSprite(manifestDirectory, outputDirectory, image, archive);
    }
}

async function convertSprite(manifestDirectory, outDir, params, archive) {
    const imagePath = path.join(manifestDirectory, params.src);

    const options = {
        hslices: 1,
        vslices: 1,
        format: "RGBA16"
    }
    Object.assign(options, params);

    const name = !!params.name ? params.name : path.basename(imagePath, path.extname(imagePath));

    const image = new N64Image(name, N64Image.Format[options.format.toUpperCase()]);
    await image.load(imagePath);

    return finalizeImage(image, manifestDirectory, outDir, options, archive, params.name)
}

async function assembleSprite(rootDir, outDir, params, archive) {
    const options = {
        format: "RGBA16"
    }
    Object.assign(options, params);

    const image = new N64Image(params.name, N64Image.Format[options.format.toUpperCase()]);

    const frames = getFrameArray(params, rootDir);
    const atlas = await buildSpriteAtlas(frames, rootDir, params);
    await image.assign(atlas);

    return finalizeImage(image, rootDir, outDir, params, archive, params.name)
}

function getDimension(params) {
    const frameSize = params.frameSize.split('x');
    return [parseInt(frameSize[0]), parseInt(frameSize[1])];
}

function createNewImage(width, height) {
    return new Promise((resolve) => {
        new Jimp(width, height, (err, img) => {
            resolve(img);
        })
    });
}

async function buildSpriteAtlas(frames, rootDir, params) {
    const [frameWidth, frameHeight] = getDimension(params);

    const atlas = await createNewImage(params.hslices * frameWidth, params.vslices * frameHeight);

    for (let y = 0; y < params.vslices; y++) {
        for (let x = 0; x < params.hslices; x++) {
            const framePath = path.join(rootDir, frames[y * params.hslices + x]);
            const frame = await Jimp.read(framePath);

            atlas.blit(frame, x * frameWidth, y * frameHeight);
        }
    }

    return atlas
}

function getFrameArray(params, rootDir) {
    if (params.frames) {
        return params.frames;
    }
    else {
        const frameSrcDir = path.join(rootDir, params.frameDir);
        const files = fs.readdirSync(frameSrcDir);
        return files.map(file => path.join(params.frameDir, file));
    }
}

async function finalizeImage(image, assetDir, outDir, options, archive) {
    if (options.resize) {
        const dimensions = options.resize.split("x");
        image.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
    }

    if (image.format === N64Image.Format.CI8 || image.format === N64Image.Format.CI4) {
        image.createColorIndexImage();

        if (options.additionalPalettes) {
            for (const paletteFile of options.additionalPalettes) {
                const paletteFilePath = path.join(assetDir, paletteFile);
                await image.colorIndexImage.addPaletteFromPath(paletteFilePath);
            }
        }
    }

    let assetIndex = -1;
    let assetBuffer = null;

    if (archive) {
        const filePath = path.join(outDir, `${image.name}.image`);
        N64ImageWriter.writeFile(image, options.hslices, options.vslices, filePath);
        const entry = await archive.add(filePath, "image");
        assetIndex = entry.index;
    }
    else {
        assetBuffer = N64ImageWriter.writeBuffer(image, options.hslices, options.vslices);
    }

    return {
        name: image.name,
        assetIndex: assetIndex,
        assetBuffer: assetBuffer,
        width: image.width,
        height: image.height,
        hslices: options.hslices,
        vslices: options.vslices,
    };
}

module.exports = processImage;