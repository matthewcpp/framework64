/* This File is not currently in use */

/* Example JSON:
{
    "name": "letter_texture",
    "atlas": {
        "rows": [
            ["image_font/048.png", "image_font/049.png"],
            ["image_font/050.png", "image_font/051.png"]
        ],
        "frameSize": "16x16"
    }
}
*/
const Jimp = require("jimp");

const fs = require("fs");
const path = require("path");

async function build(atlasJson, assetDirectory) {
    const [frameWidth, frameHeight] = _determineFrameSize(atlasJson);
    const layout = _determineLayout(atlasJson, assetDirectory);
    const maxRowSize = Math.max(...layout.map((row) => {return row.length;}));

    const atlas = new Jimp(frameWidth * maxRowSize, frameHeight * layout.length);

    const resizeFrames = Object.hasOwn(atlasJson, "resizeFrames") ? Object.resizeFrames : false;

    let destX = 0, destY = 0;
    for (const row of layout) {
        for (const imagePath of row) {
            const frame = await Jimp.read(imagePath);

            if (resizeFrames && (frame.bitmap.width !== frameWidth || frame.bitmap.height !== frameHeight)) {
                frame.resize(frameWidth, frameHeight);
            }

            atlas.blit(frame, destX, destY);
            destX += frameWidth;
        }

        destX = 0;
        destY += frameHeight;
    }
    
    return atlas;
}

function _determineFrameSize(atlasJson) {
    if (Object.hasOwn(atlasJson, "frameSize")) {
        const dimensions = atlasJson.frameSize.split('x');
        if (dimensions.length !== 2) {
            throw new Error("Image atlast 'frameSize' should be specified as WxH");
        }

        return [parseInt(dimensions[0]), parseInt(dimensions[1])];
    }
    else if (Object.hasOwn("frameWidth") && Object.hasOwn("frameHeight")){
        return [parseInt(atlasJson.frameWidth), parseInt(atlasJson.frameHeight)];
    }
    else {
        throw new Error("Image atlas needs to specify 'frameWidth' and 'frameHeight' or 'frameSize'");
    }
    
}

function _determineLayout(atlasJson, assetDirectory) {
    const layout = [];

    if (Object.hasOwn(atlasJson, "rows")) {
        for (const row of atlasJson.rows) {
            layout.push(row.map((imagePath) => { return path.join(assetDirectory, imagePath); }));
        }
    } 
    else if (Object.hasOwn(atlasJson, "dir")) {
        if (!Object.hasOwn(atlasJson, "rowSize")) {
            throw new Error("Image atlas that has 'dir' property must also have 'rowSize'");
        }

        const rowSize = parseInt(atlasJson.rowSize);

        const frameSrcDir = path.join(baseDirectory, imageJson.frameDir);
        const imageFiles = fs.readdirSync(frameSrcDir).map(file => path.join(imageJson.frameDir, file));
    
        for(let i = 0; i < imageFiles.length; i += rowSize) {
            layout.push(imageFiles.slice(i, rowSize))
        }
    }
    else {
        throw new Error("Image atlas must have either 'rows' or 'dir' property");
    }

    return layout;
}

module.exports = {
    build: build
}