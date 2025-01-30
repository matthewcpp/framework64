/* Common Image processing utils that all platforms can use */

const fs = require("fs");
const path = require("path");


/** Returns the absolute paths of all image frames for the image */
function getFramePathArray(imageJson, baseDirectory) {
    if (Object.hasOwn(imageJson, "frames")) {
        if (imageJson.frames.length < 1) {
            throw new Error("Image frame array must specify at least one path.")
        }

        return imageJson.frames.map(file => path.join(baseDirectory, file));
    }
    else {
        const frameSrcDir = path.join(baseDirectory, imageJson.frameDir);
        const files = fs.readdirSync(frameSrcDir);

        if (files.length === 0) {
            throw new Error(`frame directory: ${frameSrcDir} is empty`);
        }

        return files.map(file => path.join(baseDirectory, imageJson.frameDir, file));
    }
}

module.exports = {
    getFramePathArray: getFramePathArray
};