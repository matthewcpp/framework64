const N64Image = require("./N64Image")

const fs = require("fs");

const SizeOfSpriteHeader = 8;

function writeBinary(image, horizontalSlices, verticalSlices, path) {
    const slices = image.slice(horizontalSlices, verticalSlices);
    const sliceWidth = image.width / horizontalSlices;
    const sliceHeight = image.height / verticalSlices;

    const headerBuffer = Buffer.alloc(SizeOfSpriteHeader);
    let bufferOffset = 0;

    // write the header info
    bufferOffset = headerBuffer.writeUInt16BE(image.width, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(image.height, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(horizontalSlices, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(verticalSlices, bufferOffset);

    const file = fs.openSync(path, "w");
    fs.writeSync(file, headerBuffer);

    for (const slice of slices.images) {
        fs.writeSync(file, N64Image.encode16bpp(slice, sliceWidth, sliceHeight));
    }

    fs.closeSync(file);

}

module.exports = {
    write: writeBinary
}