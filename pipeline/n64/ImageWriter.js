const Image = require("./Image");
const ColorIndexImage = require("../ColorIndexImage");

const fs = require("fs");

function writeFile(image, horizontalSlices, verticalSlices, path) {
    const file = fs.openSync(path, "w");
    fs.writeSync(file, buildImageBuffer(image, horizontalSlices, verticalSlices));
    fs.closeSync(file);
}

function writeBuffer(image, horizontalSlices, verticalSlices) {
    return buildImageBuffer(image, horizontalSlices, verticalSlices);
}

function buildImageBuffer(image, horizontalSlices, verticalSlices) {
    const slices = image.slice(horizontalSlices, verticalSlices);
    const sliceWidth = image.width / horizontalSlices;
    const sliceHeight = image.height / verticalSlices;

    const buffers = [];

    buffers.push(createImageHeaderBuffer(image, horizontalSlices, verticalSlices));

    switch (image.format) {
        case Image.Format.IA8:
            for (const slice of slices.images) {
                buffers.push(encodeIA8Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case Image.Format.IA4:
            for (const slice of slices.images) {
                buffers.push(encodeIA4Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case Image.Format.RGBA16:
            for (const slice of slices.images) {
                buffers.push(encode16bppSlice(slice, sliceWidth, sliceHeight));
            }
            break;

        case Image.Format.RGBA32:
            for (const slice of slices.images) {
                buffers.push(encodeRGBA32Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case Image.Format.I8:
            for (const slice of slices.images) {
                buffers.push(encodeI8Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case Image.Format.I4:
            for (const slice of slices.images) {
                buffers.push(encodeI4Slice(slice, sliceWidth, sliceHeight));
            }
            break;

        case Image.Format.CI8:
            validateCIPalette(image);
            for (const slice of slices.images) {
                buffers.push(encodeCI8Slice(slice, sliceWidth, sliceHeight, image.colorIndexImage));
            }
            buffers.push(buildColorIndexPalettesBuffer(image));
            break;

        case Image.Format.CI4:
            validateCIPalette(image);
            for (const slice of slices.images) {
                buffers.push(encodeCI4Slice(slice, sliceWidth, sliceHeight, image.colorIndexImage));
            }
            buffers.push(buildColorIndexPalettesBuffer(image));
            break;
    }

    return Buffer.concat(buffers);
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

function encodeCI4Slice(slice, sliceWidth, sliceHeight, colorIndexImage) {
    const bufferSize = (sliceWidth * sliceHeight) / 2;
    const buffer = Buffer.alloc(bufferSize);


    for (let i = 0; i < bufferSize; i++) {
        const index = i * 8;
        const colorIndex1 = colorIndexImage.getColorIndex(slice[index    ], slice[index + 1], slice[index + 2], slice[index + 3]);
        const colorIndex2 = colorIndexImage.getColorIndex(slice[index + 4], slice[index + 5], slice[index + 6], slice[index + 7]);

        if (typeof(colorIndex1) === "undefined" || typeof(colorIndex2) === "undefined") {
            throw new Error("Error building Color index image.  Could not locate color in lookup table");
        }

        buffer.writeUInt8((colorIndex1 << 4) | colorIndex2, i);
    }

    return buffer;
}

function validateCIPalette(image) {
    const colorIndexImage = image.colorIndexImage;

    let maxPaletteSize = 0;
    if (image.format == Image.Format.CI8) {
        maxPaletteSize = 256;
    }
    else if (image.format == Image.Format.CI4) {
        maxPaletteSize = 16;
    }
    else {
        throw new Error(`Error validating CI palette: Unknown format: ${image.format}`);
    }

    for (let i = 0; i < colorIndexImage.palettes.length; i++) {
        if (colorIndexImage.palettes[i].length > maxPaletteSize) {
            throw new Error(`Palette ${i} contains ${colorIndexImage.palettes[i].length} colors (max ${maxPaletteSize})`);
        }
    }
}

function buildColorIndexPalettesBuffer(image) {
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

    return dataBuffer;
}

module.exports = {
    writeFile: writeFile,
    writeBuffer: writeBuffer,
}
