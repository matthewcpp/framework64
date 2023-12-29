const ImageConvert = require("./ImageConvert");

const path = require("path");

async function processImage(imageJson, bundle, assetDirectory, outputDirectory) {
    const runingInDocker = Object.hasOwn(process.env, "FW64_N64_LIBDRAGON_DOCKER_CONTAINER");

    if (runingInDocker) {
        throw new Error("docker not supported, yet");
    }

    await ImageConvert.convertLibdragonSprite(imageJson.src, assetDirectory, outputDirectory);

    const name = !!imageJson.name ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src));
    const spriteFile = ImageConvert.getSpriteNameForImage(imageJson.src);
    const spritePath = path.join(outputDirectory, spriteFile);

    bundle.addImage(name, spritePath);
}

module.exports = processImage;