const fs = require("fs");
const Image = require("./Image");
const N64LibUltraImageWriter = require("./ImageWriter");

class N64LibUltraFontWriter {
    static SizeOfFontHeader = 12;
    static SizeOfFontGlyph = 8;

    async writeFile(font, path) {
        const imageBuffer = await this._getOrCreateImageBuffer(font);
        const headerBuffer = Buffer.alloc(N64LibUltraFontWriter.SizeOfFontHeader);
        let bufferOffset = 0;

        // write the header info. This needs to be kept in sync with fw64N64FontInfo in n64_libultra/font.h
        bufferOffset = headerBuffer.writeUInt32BE(font.size, bufferOffset);
        bufferOffset = headerBuffer.writeUInt32BE(font.glyphs.length, bufferOffset);
        bufferOffset = headerBuffer.writeUInt32BE(font.lineHeight, bufferOffset);

        const glyphBuffer = Buffer.alloc(N64LibUltraFontWriter.SizeOfFontGlyph * font.glyphs.length);
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
        fs.writeSync(file, imageBuffer);
        fs.closeSync(file);
    }

    async _getOrCreateImageBuffer(font) {
        if (font.isImageFont) {
            // this image processor returns image details. In the case of libultra, this also contains an asset buffer of the image
            return font.image.assetBuffer;
        } else {
            // create the image of all the glyphs and write it to a buffer
            const image = await font.createFontImage(Image.Format.IA8);
            const hslices = image.width / font.tileWidth;
            const vslices = image.height / font.tileHeight;
            const imageWriter = new N64LibUltraImageWriter();
            return imageWriter.writeBuffer(image, hslices, vslices);
        }
    }
};

module.exports = N64LibUltraFontWriter
