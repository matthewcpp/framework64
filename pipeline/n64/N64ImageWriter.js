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
        case N64Image.Format.IA8:
        case N64Image.Format.IA4:
            bufferOffset = headerBuffer.writeUInt16BE(1, bufferOffset);
            break;

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
        case N64Image.Format.IA8:
            for (const slice of slices.images) {
                fs.writeSync(file, encodeIA8Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case N64Image.Format.IA4:
            for (const slice of slices.images) {
                fs.writeSync(file, encodeIA4Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case N64Image.Format.RGBA16:
            for (const slice of slices.images) {
                fs.writeSync(file, encode16bppSlice(slice, sliceWidth, sliceHeight));
            }
            break;

        case N64Image.Format.RGBA32:
            for (const slice of slices.images) {
                fs.writeSync(file, encodeRGBA32Slice(slice));
            }
            break;
    }

    fs.closeSync(file);
}

function encode16bppSlice(data, width, height) {
    const pixelCount = width * height;

    // rgba data (2 bytes per pixel)
    const bufferSize = pixelCount * 2;

    const buffer = Buffer.alloc(bufferSize);
    let offset = 0;

    for (let i = 0; i < pixelCount; i++) {
        let index = i * 4;

        // note the pixel data format is 5r 5g 5b 1a
        const r = data[index] >> 3;
        const g = data[index + 1] >> 3;
        const b = data[index + 2] >> 3;
        const a = data[index + 3] === 0 ? 0 : 1;

        const val = r << 11 | g << 6 | b << 1 | a;
        offset = buffer.writeUInt16BE(val, offset);
    }

    return buffer;
}

function encodeRGBA32Slice(data) {
    const buffer = Buffer.alloc(data.length);

    for (let i = 0; i < data.length; i++) {
        buffer.writeUInt8(data[i], i);
    }

    return buffer;
}

function encodeIA8Slice(data, sliceWidth, sliceHieght) {
    const textelCount = sliceWidth * sliceHieght;
    const buffer = Buffer.alloc(textelCount);

    for(let i = 0; i < textelCount; i++) {
        const index = i * 4;
        const intensity = parseInt((data[index] / 255) * 15);
        const alpha = parseInt((data[index + 3] / 255) * 15);
        const value = (intensity << 4) | alpha;
        buffer.writeUInt8(value, i);
    }

    return buffer;
}

function encodeIA4Slice(data, sliceWidth, sliceHieght) {
    const textelCount = (sliceWidth * sliceHieght) / 2;
    const buffer = Buffer.alloc(textelCount);

    for(let i = 0; i < textelCount; i++) {
        const index = i * 8;
        const intensity1 = parseInt((data[index] / 255) * 7);
        const alpha1 = data[index + 3] !== 0 ? 1 : 0;
        const value1 = (intensity1 << 1) | alpha1;

        const intensity2 = parseInt((data[index + 4] / 255) * 7);
        const alpha2 = data[index + 7] !== 0 ? 1 : 0;
        const value2 = (intensity2 << 1) | alpha2;

        const value = (value1 << 4) | value2;

        buffer.writeUInt8(value, i);
    }

    return buffer;
}

module.exports = {
    writeBinary: writeBinary
}