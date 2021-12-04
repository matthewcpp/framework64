const N64Image = require("./N64Image");

const fs = require("fs");

const SizeOfFontHeader = 8;
const SizeOfFontGlyph = 8;

function writeBinary(font, imageIndex, path) {
    const headerBuffer = Buffer.alloc(SizeOfFontHeader);
    let bufferOffset = 0;

    // write the header info
    bufferOffset = headerBuffer.writeUInt16BE(font.size, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(font.glyphs.length, bufferOffset);
    bufferOffset = headerBuffer.writeUInt32BE(imageIndex, bufferOffset);

    const glyphBuffer = Buffer.alloc(SizeOfFontGlyph * font.glyphs.length);
    bufferOffset = 0;

    for (const glyph of font.glyphs) {
        bufferOffset = glyphBuffer.writeUInt32BE(glyph.codepoint, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.top, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.left, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.advance, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.height, bufferOffset);
    }

    const file = fs.openSync(path, "w");
    fs.writeSync(file, headerBuffer);
    fs.writeSync(file, glyphBuffer);
    fs.closeSync(file);
}

module.exports = {
    write: writeBinary
}