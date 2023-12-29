const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const mkspritePath = "/opt/libdragon/bin/mksprite";
const dockerMappedInDir = "/tmp/in";
const dockerMappedOutDir = "/tmp/out";


async function convertLibdragonSprite(sourceFile, assetDirectory, outputDirectory) {
    // Note we always want to ensure '/' directory separators
    const sourcePath = path.join(dockerMappedInDir, sourceFile).replaceAll('\\', '/');

    const dockerArgs = [
        "run", "--rm",
        "-v", `${assetDirectory}:${dockerMappedInDir}`,
        "-v", `${outputDirectory}:${dockerMappedOutDir}`,
        "matthewcpp/framework64-n64-libdragon",
        mkspritePath,
        "--output", dockerMappedOutDir,
        "--compress", "0",
        sourcePath
    ];

    await execFile("docker", dockerArgs);
}

function getSpriteNameForImage(imagePath) {
    return path.join(path.dirname(imagePath), path.basename(imagePath, path.extname(imagePath)) + ".sprite");
}

module.exports = {
    convertLibdragonSprite: convertLibdragonSprite,
    getSpriteNameForImage: getSpriteNameForImage
};
