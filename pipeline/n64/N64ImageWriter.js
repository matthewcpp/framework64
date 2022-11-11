const N64Image = require("./N64Image");
const ColorIndexImage = require("../ColorIndexImage");

const fs = require("fs");

function writeBinary(image, horizontalSlices, verticalSlices, path) {
    const slices = image.slice(horizontalSlices, verticalSlices);
    const sliceWidth = image.width / horizontalSlices;
    const sliceHeight = image.height / verticalSlices;

    const headerBuffer = createImageHeaderBuffer(image, horizontalSlices, verticalSlices);
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
                fs.writeSync(file, encodeRGBA32Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case N64Image.Format.I8:
            for (const slice of slices.images) {
                fs.writeSync(file, encodeI8Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case N64Image.Format.I4:
            for (const slice of slices.images) {
                fs.writeSync(file, encodeI4Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case N64Image.Format.CI8:
            validateCIPalette(image);
            for (const slice of slices.images) {
                fs.writeSync(file, encodeCI8Slice(slice, sliceWidth, sliceHeight, image.colorIndexImage));
            }
            writePalette(image, file)
            break;
    }

    fs.closeSync(file);
}

function createImageHeaderBuffer(image, horizontalSlices, verticalSlices) {
    const headerBuffer = Buffer.alloc(14);
    let bufferOffset = 0;

    let paletteCount = 0, paletteSize = 0;
    if (image.colorIndexImage) {
        paletteCount = image.colorIndexImage.palettes.length;
        paletteSize = image.colorIndexImage.primaryPalette.length;
    }

    bufferOffset = headerBuffer.writeUInt16BE(image.format, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(image.width, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(image.height, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(horizontalSlices, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(verticalSlices, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(paletteCount, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(paletteSize, bufferOffset);

    return headerBuffer;
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

function encodeRGBA32Slice(data, sliceWidth, sliceHeight) {
    const buffer = Buffer.alloc(sliceWidth * sliceHeight * 4);

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

function encodeI8Slice(data, sliceWidth, sliceHieght) {
    const textelCount = sliceWidth * sliceHieght;
    const buffer = Buffer.alloc(textelCount);

    for(let i = 0; i < textelCount; i++) {
        const index = i * 4;
        buffer.writeUInt8(data[index], i);
    }

    return buffer;
}

function encodeI4Slice(data, sliceWidth, sliceHieght) {
    const textelCount = (sliceWidth * sliceHieght) / 2;
    const buffer = Buffer.alloc(textelCount);

    for(let i = 0; i < textelCount; i++) {
        const index = i * 8;
        const intensity1 = parseInt((data[index] / 255) * 15);
        const intensity2 = parseInt((data[index + 4] / 255) * 15);

        const value = (intensity1 << 4) | intensity2;

        buffer.writeUInt8(value, i);
    }

    return buffer;
}

function encodeCI8Slice(slice, sliceWidth, sliceHeight, colorIndexImage) {
    const textelCount = sliceWidth * sliceHeight;
    const buffer = Buffer.alloc(textelCount);

    for (let i = 0; i < textelCount; i++) {
        const index = i * 4;
        const colorIndex = colorIndexImage.getColorIndex(slice[index], slice[index + 1], slice[index + 2], slice[index + 3]);

        if (typeof(colorIndex) === "undefined") {
            throw new Error("Error building Color index image.  Could not locate color in lookup table");
        }

        buffer.writeUInt8(colorIndex, i);
    }

    return buffer;
}

function validateCIPalette(image) {
    const colorIndexImage = image.colorIndexImage;

    let maxPaletteCount = 0;
    if (image.format == N64Image.Format.CI8) {
        maxPaletteCount = 256;
    }

    for (let i = 0; i < colorIndexImage.palettes; i++) {
        if (colorIndexImage.palettes[i].length >= maxLength) {
            throw new Error(`Palette ${i} contains ${colorIndexImage.palettes[i].length} colors (max ${maxLength})`);
        }
    }
}

function writePalette(image, file) {
    const colorIndexImage = image.colorIndexImage;
    
    // each color in the palette is represented by a 5/5/5/1 color
    const paletteSizeInBytes = colorIndexImage.primaryPalette.length * 2;
    const dataBuffer = Buffer.alloc(colorIndexImage.palettes.length * paletteSizeInBytes);
    bufferOffset = 0;

    for (const palette of colorIndexImage.palettes) {
        for (const color of palette) {
            const r = color[0] >> 3;
            const g = color[1] >> 3;
            const b = color[2] >> 3;
            const a = color[3] === 0 ? 0 : 1;
    
            const val = r << 11 | g << 6 | b << 1 | a;
            bufferOffset = dataBuffer.writeUint16BE(val, bufferOffset);
        }
    }

    fs.writeSync(file, dataBuffer);
}

module.exports = {
    writeBinary: writeBinary,
    writePalette: writePalette
}
