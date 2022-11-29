const Util = require("./Util");

const fs = require("fs");
const path = require("path");

function writeHeaderFile(imageJson, assetDirectory, outputDirectory) {
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const safeName = Util.safeDefineName(imageJson.name);

    const includeFilePath = path.join(includeDirectory, `image_atlas_${safeName}.h`);

    const file = fs.openSync(includeFilePath, 'w');
    fs.writeSync(file, "#pragma once\n\n");

    let frameIndex = 0;
    const frames = getFrameNames(imageJson, assetDirectory);
    for(const framePath of frames) {
        const frameName = Util.safeDefineName(path.basename(framePath, path.extname(framePath)));
        fs.writeSync(file, `#define image_atlas_${safeName}_frame_${frameName} ${frameIndex}\n`);
        frameIndex += 1;
    }

    fs.writeSync(file, `\n#define image_atlas_${safeName}_num_frames ${frameIndex}\n`);

    fs.closeSync(file);
}

function getFrameNames(imageJson, assetDirectory){
    if (imageJson.frames) {
        return imageJson.frames;
    } 
    else {
        const atlasPath = path.join(assetDirectory, imageJson.frameDir);
        return fs.readdirSync(atlasPath);
    }
}

module.exports = {
    writeHeaderFile: writeHeaderFile
}