const fs = require("fs");

const SizeOfFontHeader = 4;
const SizeOfFontGlyph = 8;

function writeBinary(fontData, imageBuffer, path) {
    const headerBuffer = Buffer.alloc(SizeOfFontHeader);
    let bufferOffset = 0;

    // write the header info
    bufferOffset = headerBuffer.writeUInt16BE(fontData.size, bufferOffset);
    bufferOffset = headerBuffer.writeUInt16BE(fontData.glyphs.length, bufferOffset);

    const glyphBuffer = Buffer.alloc(SizeOfFontGlyph * fontData.glyphs.length);
    bufferOffset = 0;

    for (const glyph of fontData.glyphs) {
        bufferOffset = glyphBuffer.writeUInt32BE(glyph.codepoint, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.top, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.left, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.advance, bufferOffset);
        bufferOffset = glyphBuffer.writeInt8(glyph.height, bufferOffset);
    }

    const file = fs.openSync(path, "w");
    fs.writeSync(file, headerBuffer);
    fs.writeSync(file, glyphBuffer);
    fs.writeSync(file, imageBuffer);
    fs.closeSync(file);
}

module.exports = {
    write: writeBinary
}