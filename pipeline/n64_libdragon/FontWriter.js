const LibDragonImageWriter = require("./ImageWriter");

const fs = require("fs");
const path = require("path");

class N64LibDragonFontWriter {
    static SizeOfFontHeader = 12;
    static SizeOfFontGlyph = 8;

    async writeFile(font, destPath) {
        const imageWriter = new LibDragonImageWriter();
        // write all the glyphs into slices that will fit into tmem
        const glyphImages = await font.createGlyphImages();

        const outDir = path.dirname(destPath);
        let sliceIndex = 0;
        const libdragonSpritePaths = [];
        for (const glyphImage of glyphImages) {
            const imagePath = path.join(outDir, `font${sliceIndex++}.png`);
            console.log("write slice: " + imagePath);
            await glyphImage.writeToFile(imagePath);
            libdragonSpritePaths.push(await imageWriter.writeLibdragonSprite(imagePath, "IA8"));
        }

        // This needs to be kept in sync with fw64LibDragonFontInfo in libdragon_font.h
        const fontInfoBuffer = Buffer.alloc(N64LibDragonFontWriter.SizeOfFontHeader);
        fontInfoBuffer.writeUInt32BE(font.size, 0);
        fontInfoBuffer.writeUInt32BE(font.lineHeight, 4);
        fontInfoBuffer.writeUInt32BE(font.glyphs.length, 8);

        // This needs to be kept in sync with fw64LibDragonFontGlyph in libdragon_font.h
        const glyphBuffer = Buffer.alloc(N64LibDragonFontWriter.SizeOfFontGlyph * font.glyphs.length);
        let bufferOffset = 0;

        for (const glyph of font.glyphs) {
            bufferOffset = glyphBuffer.writeUInt32BE(glyph.codepoint, bufferOffset);
            bufferOffset = glyphBuffer.writeInt8(glyph.top, bufferOffset);
            bufferOffset = glyphBuffer.writeInt8(glyph.left, bufferOffset);
            bufferOffset = glyphBuffer.writeInt8(glyph.advance, bufferOffset);
            bufferOffset = glyphBuffer.writeInt8(glyph.height, bufferOffset);
        }

        // the writing order needs to be kept in sync with reading order in fw64_font_load_from_datasource in libdragon_font.c
        const file = fs.openSync(destPath, "w");
        fs.writeSync(file, fontInfoBuffer);
        imageWriter.writeImageSlicesToFile(libdragonSpritePaths, libdragonSpritePaths.length, 1, file);
        fs.writeSync(file, glyphBuffer);

        fs.closeSync(file);
    }
};

module.exports = N64LibDragonFontWriter;
