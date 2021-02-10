const N64Image = require("./N64Image")
const N64ImageWriter = require("./N64ImageWriter");

const fs = require("fs");
const path = require("path");

function writeSpriteHeader(image, slices, path) {
    const header = fs.openSync(path, "w");

    fs.writeSync(header, `#ifndef ${image.name}_SPRITE_H\n`);
    fs.writeSync(header, `#define ${image.name}_SPRITE_H\n\n`);

    fs.writeSync(header, `#define ${image.name}_SPRITE_WIDTH ${image.width}\n`);
    fs.writeSync(header, `#define ${image.name}_SPRITE_HEIGHT ${image.height}\n`);

    fs.writeSync(header, `#define ${image.name}_SPRITE_HSLICES ${slices.hslices}\n`);
    fs.writeSync(header, `#define ${image.name}_SPRITE_VSLICES ${slices.vslices}\n`);

    fs.writeSync(header, `#define ${image.name}_SPRITE_SLICE_COUNT ${slices.images.length}\n\n`);

    fs.writeSync(header, `extern unsigned char* ${image.name}_sprite_slices[];\n\n`);

    fs.writeSync(header, "#endif\n");

    fs.closeSync(header);
}

function writeSpriteSource(image, slices, path, includePath) {
    const source = fs.openSync(path, "w");
    fs.writeSync(source, `#include "${includePath}"\n\n`);

    for (let i = 0; i < slices.images.length; i++) {
        const slice_name = `${image.name}_slice_${i}`;
        const sliceData = slices.images[i];

        const sliceWidth = image.width / slices.hslices;
        const sliceHeight = image.height / slices.vslices;
        const buffer = N64Image.encode16bpp(sliceData, sliceWidth, sliceHeight);
        N64ImageWriter.writeDataArray(source, slice_name, buffer, sliceWidth, sliceHeight);
    }

    fs.writeSync(source, "\n\n");

    fs.writeSync(source, `unsigned char* ${image.name}_sprite_slices[] = {\n`);
    for (let i = 0; i < slices.images.length; i++) {
        fs.writeSync(source, `${image.name}_slice_${i},\n`);
    }
    fs.writeSync(source, "};\n\n");

    fs.closeSync(source);
}

function writeSprite(image, hslices, vslices, outDir) {
    const filename = image.name + "_sprite"
    const headerFileName = `${filename}.h`;
    const sourceFileName = `${filename}.c`;

    const headerPath = path.join(outDir, headerFileName);
    const sourcePath = path.join(outDir, sourceFileName);

    const slices = image.slice(hslices, vslices);

    writeSpriteHeader(image, slices, headerPath);
    writeSpriteSource(image, slices, sourcePath, headerFileName);
}

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