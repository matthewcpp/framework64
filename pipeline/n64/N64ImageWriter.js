const N64Image = require("./N64Image");

const fs = require("fs");

function writeBinary(image, horizontalSlices, verticalSlices, path) {
    const slices = image.slice(horizontalSlices, verticalSlices);
    const sliceWidth = image.width / horizontalSlices;
    const sliceHeight = image.height / verticalSlices;

    const headerBuffer = Buffer.alloc(12);
    let bufferOffset = 0;

    bufferOffset = headerBuffer.writeUInt16BE(image.format, bufferOffset);

    switch (image.format) {
        case N64Image.Format.RGBA16:
            bufferOffset = headerBuffer.writeUInt16BE(2, bufferOffset);
            break;

        case N64Image.Format.RGBA32:
            bufferOffset = headerBuffer.writeUInt16BE(4, bufferOffset);
            break;
    }

    bufferOffset = headerBuffer.writeUInt16BE(image.width, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(image.height, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(horizontalSlices, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(verticalSlices, bufferOffset);

    const file = fs.openSync(path, "w");
    fs.writeSync(file, headerBuffer);

    switch (image.format) {
        case N64Image.Format.RGBA16:
            for (const slice of slices.images) {
                fs.writeSync(file, N64Image.encode16bpp(slice, sliceWidth, sliceHeight));
            }
            break;

        case N64Image.Format.RGBA32:
            for (const slice of slices.images) {
                fs.writeSync(file, N64Image.encodeRGBA32(slice));
            }
            break;
    }

    fs.closeSync(file);
}

module.exports = {
    writeBinary: writeBinary
}