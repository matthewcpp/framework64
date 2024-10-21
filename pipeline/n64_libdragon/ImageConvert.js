const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const mkspritePath = "/opt/libdragon/bin/mksprite";
const dockerMappedInDir = "/tmp/in";
const dockerMappedOutDir = "/tmp/out";


async function convertLibdragonSprite(sourceFile, format, assetDirectory, outputDirectory) {
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
        "-f", format,
        sourcePath
    ];

    await execFile("docker", dockerArgs);

    return getSpriteNameForImage(outputDirectory, sourceFile);
}

function getSpriteNameForImage(directoryPath, imagePath) {
    return path.join(directoryPath, path.basename(imagePath, path.extname(imagePath)) + ".sprite");
}

module.exports = {
    convertLibdragonSprite: convertLibdragonSprite,
};
