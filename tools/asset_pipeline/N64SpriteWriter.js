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
        N64ImageWriter.writeDataArray(source, slice_name, sliceData, image.width / slices.hslices, image.height / slices.vslices);
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

module.exports = {
    write: writeSprite
}