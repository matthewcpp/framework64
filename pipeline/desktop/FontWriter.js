const ImageWriter = require("./ImageWriter");

const fs = require("fs");

async function writeFile(font, dest_path) {
    const fontHeader = new FontHeader(font);

    const fontFile = fs.openSync(dest_path, "w");
    fs.writeSync(fontFile, fontHeader.buffer);
    fs.writeSync(fontFile, font.desktopGlyphBuffer);
    await ImageWriter.writeToOpenStream(font.image, fontFile);
    fs.closeSync(fontFile);
}

// this class should correspond to FontHeader in desktop/font.cpp
class FontHeader {
    static BufferSize = 12;

    buffer = Buffer.alloc(FontHeader.BufferSize);

    constructor(font) {
        this.assign(font);
    }

    assign(font) {
        let index = 0;

        index = this.buffer.writeUint32LE(font.size, index);
        index = this.buffer.writeUint32LE(font.glyphs.length, index);
        index = this.buffer.writeUint32LE(font.lineHeight, index);
    }
}

module.exports = {
    writeFile: writeFile
};