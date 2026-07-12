const DesktopImageWriter = require("./ImageWriter");

const fs = require("fs");

class DesktopFontWriter {
    async writeFile(font, dest_path) {
        // image fonts do not need to write glyps to a canvas, they are already stitched together in a grid
        if (!font.isImageFont) {
            await font.createGlImage();
        }

        const fontHeader = new FontHeader(font);

        const fontFile = fs.openSync(dest_path, "w");
        fs.writeSync(fontFile, fontHeader.buffer);
        fs.writeSync(fontFile, font.desktopGlyphBuffer);
        const imageWriter = new DesktopImageWriter();
        await imageWriter.writeToOpenStream(font.image, fontFile);
        fs.closeSync(fontFile);
    }
};

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

module.exports = DesktopFontWriter;