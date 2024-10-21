const ImageConvert = require("./ImageConvert");
const Util = require("../Util");

const fs = require("fs");
const path = require("path");

async function processImage(imageJson, dfsBundle, assetDirectory, outputDirectory, pluginMap) {
    const runingInDocker = Object.hasOwn(process.env, "FW64_N64_LIBDRAGON_DOCKER_CONTAINER");

    if (runingInDocker) {
        throw new Error("docker not supported, yet");
    }

    const name = Util.safeDefineName(!!imageJson.name ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src)));
    const assetId = dfsBundle.addImage(path.join(assetDirectory, imageJson.src), name);

    const format = Object.hasOwn(imageJson, "format") ? imageJson.format.toUpperCase() : "RGBA16";

    const convertedName = await ImageConvert.convertLibdragonSprite(imageJson.src, format, assetDirectory, outputDirectory);
    const renamedAssetPath = path.join(outputDirectory, assetId.toString());

    if (!fs.existsSync(convertedName)) {
        throw new Error(`Unable to locate libdragon converted sprite: ${convertedName}`);
    }

    fs.renameSync(convertedName, renamedAssetPath);

    return assetId;
}

module.exports = processImage;