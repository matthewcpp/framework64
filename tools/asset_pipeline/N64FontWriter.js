const N64Image = require("./N64Image");
const N64ImageWriter = require("./N64ImageWriter");

const fs = require("fs");
const path = require("path");

function writeFontHeader(font, path) {
    const header = fs.openSync(path, "w");

    fs.writeSync(header, `#ifndef ${font.name}_FONT_H\n`);
    fs.writeSync(header, `#define ${font.name}_FONT_H\n\n`);

    fs.writeSync(header, '#include "ultra/font.h"\n\n');

    fs.writeSync(header, `#define ${font.name}_FONT_SIZE ${font.size}\n`);
    fs.writeSync(header, `#define ${font.name}_FONT_GLYPH_COUNT ${font.glyphs.length}\n`);
    fs.writeSync(header, `extern FontGlyph ${font.name}_glyphs[];\n\n`);

    fs.writeSync(header, `#define ${font.name}_SPRITEFONT_TILE_WIDTH ${font.tileWidth}\n`);
    fs.writeSync(header, `#define ${font.name}_SPRITEFONT_TILE_HEIGHT ${font.tileHeight}\n`);
    fs.writeSync(header, `extern unsigned char ${font.name}_spritefont[];\n\n`);

    fs.writeSync(header, "#endif\n");

    fs.closeSync(header);
}

function writeFontSource(font, path, includePath) {
    const source = fs.openSync(path, "w");
    fs.writeSync(source, `#include "${includePath}"\n\n`);

    fs.writeSync(source, `FontGlyph ${font.name}_glyphs[] = {\n`);
    for (const glyph of font.glyphs) {
        fs.writeSync(source, `{${glyph.codepoint}, ${glyph.top}, ${glyph.left}, ${glyph.advance}, ${glyph.height}},\n`);
    }
    fs.writeSync(source, "};\n\n");

    N64ImageWriter.writeDataArray(source, `${font.name}_spritefont`, font.image.data, font.image.width, font.image.height);

    fs.closeSync(source);
}

function writeFont(font, outDir) {
    const headerFileName = `${font.name}.h`;
    const sourceFileName = `${font.name}.c`;

    writeFontHeader(font, path.join(outDir, headerFileName));
    writeFontSource(font, path.join(outDir, sourceFileName), headerFileName);
}

module.exports = {
    writeFont: writeFont
}