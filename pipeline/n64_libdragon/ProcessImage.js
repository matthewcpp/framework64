const Util = require("../Util");

const fs = require("fs");
const path = require("path");
const util = require("util");

const execFile = util.promisify(require('child_process').execFile);

const mkspritePath = "/opt/libdragon/bin/mksprite";

async function processImage(imageJson, dfsBundle, assetDirectory, outputDirectory) {
    const name = Util.safeDefineName(Object.hasOwn(imageJson, "name") ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src)));
    const format = Object.hasOwn(imageJson, "format") ? imageJson.format.toUpperCase() : "RGBA16";
    const sourcePath = path.join(assetDirectory, imageJson.src);
    const assetId = dfsBundle.addImage(sourcePath, name);

    const mkspriteArgs = [
        "--output", outputDirectory,
        "--compress", "0",
        "-f", format,
        sourcePath
    ];

    // console.log(">>>>>>>>>>>>>>>>>>>>>>>>>> mksprite");
    // console.log(mkspriteArgs.join(' '));
    // console.log(">>>>>>>>>>>>>>>>>>>>>>>>>> mksprite");
    
    await execFile(mkspritePath, mkspriteArgs);

    const convertedSpriteFile = path.join(outputDirectory, path.basename(sourcePath, path.extname(sourcePath)) + ".sprite");
    const convertedAssetIdFile = path.join(outputDirectory, assetId.toString());

    if (!fs.existsSync(convertedSpriteFile)) {
        throw new Error(`Error converting sprite: ${sourcePath}`);
    }

    fs.renameSync(convertedSpriteFile, convertedAssetIdFile);

    return assetId;
}

module.exports = processImage;