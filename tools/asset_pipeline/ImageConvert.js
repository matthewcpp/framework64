const N64Image = require("./N64Image")
const N64SpriteWriter = require("./N64SpriteWriter");

const Jimp = require("jimp");

const path = require("path");

function finalizeImage(image, outDir, options) {
    if (options.resize) {
        const dimensions = options.resize.split("x");
        image.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
    }

    N64SpriteWriter.write(image, options.hslices, options.vslices, outDir);
}

async function convertSprite(imagePath, outDir, params) {
    const options = {
        hslices: 1,
        vslices: 1
    }
    Object.assign(options, params);

    const name = path.basename(imagePath, path.extname(imagePath));

    const image = new N64Image(name);
    await image.load(imagePath);

    finalizeImage(image, outDir, options)
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

// TODO: Validate params
async function assembleSprite(rootDir, outDir, params) {
    const [frameWidth, frameHeight] = getDimension(params);

    const atlas = await createNewImage(params.hslices * frameWidth, params.vslices * frameHeight);

    for (let y = 0; y < params.vslices; y++) {
        for (let x = 0; x < params.hslices; x++) {
            const framePath = path.join(rootDir, params.frames[y * params.hslices + x]);
            const frame = await Jimp.read(framePath);

            atlas.blit(frame, x * frameWidth, y * frameHeight);
        }
    }

    const image = new N64Image(params.name);
    await image.assign(atlas);

    finalizeImage(image, outDir, params)
    image.writeToFile("D:/temp/atlas.png");
}

module.exports = {
    convertSprite: convertSprite,
    assembleSprite: assembleSprite,
}