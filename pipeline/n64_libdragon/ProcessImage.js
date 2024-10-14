const ImageConvert = require("./ImageConvert");

const fs = require("fs");
const path = require("path");

async function processImage(imageJson, dfsBundle, assetDirectory, outputDirectory) {
    const runingInDocker = Object.hasOwn(process.env, "FW64_N64_LIBDRAGON_DOCKER_CONTAINER");

    if (runingInDocker) {
        throw new Error("docker not supported, yet");
    }

    const name = !!imageJson.name ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src));
    const spriteFileName = dfsBundle.addImage(name);
    
    const convertedName = await ImageConvert.convertLibdragonSprite(imageJson.src, assetDirectory, outputDirectory);
    const renamedAssetPath = path.join(outputDirectory, spriteFileName);
    console.log(convertedName);
    fs.renameSync(convertedName, renamedAssetPath);
    
}

module.exports = processImage;