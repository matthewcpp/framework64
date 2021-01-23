const N64Image = require("./N64Image")
const N64SpriteWriter = require("./N64SpriteWriter");

const path = require("path");

async function convertSprite(imagePath, outDir, params) {
    const options = {
        hslices: 1,
        vslices: 1
    }
    Object.assign(options, params);

    const name = path.basename(imagePath, path.extname(imagePath));

    const image = new N64Image(name);
    await image.load(imagePath);

    if (params.resize) {
        const dimensions = params.resize.split("x");
        image.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
    }

    N64SpriteWriter.write(image, options.hslices, options.vslices, outDir);
}
module.exports = {
    convertSprite: convertSprite
}