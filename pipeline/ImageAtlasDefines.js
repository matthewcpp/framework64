const Util = require("./Util");

const fs = require("fs");
const path = require("path");

function writeHeaderFile(image, outputDirectory) {
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const safeName = Util.safeDefineName(image.name);

    const includeFilePath = path.join(includeDirectory, `image_atlas_${safeName}.h`);

    const file = fs.openSync(includeFilePath, 'w');
    fs.writeSync(file, "#pragma once\n\n");

    let frameIndex = 0;
    for(const framePath of image.frames) {
        const frameName = Util.safeDefineName(path.basename(framePath, path.extname(framePath)));
        fs.writeSync(file, `#define image_atlas_${safeName}_frame_${frameName} ${frameIndex}\n`);
        frameIndex += 1;
    }

    fs.writeSync(file, `\n#define image_atlas_${safeName}_num_frames ${frameIndex}\n`);

    fs.closeSync(file);
}

module.exports = {
    writeHeaderFile: writeHeaderFile
}