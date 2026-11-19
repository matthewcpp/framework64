const LibDragonImageWriter = require("./ImageWriter");

const fs = require("fs");
const path = require("path");
const { buffer } = require("stream/consumers");

class N64LibDragonFontWriter {
    static SizeOfFontHeader = 16;
    static SizeOfFontGlyph = 4 /* codepoint */ + 2 + 2/* frame and index */ + 4 /* metrics */;

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

        // Track the position of this glyph in the image.  Note: they are written to the image in the
        // same order that they appear in the glyph array.
        const glyphsPerSlice = font.horizontalGlyphCount * font.verticalGlyphCount;

        // This needs to be kept in sync with fw64LibDragonFontInfo in libdragon_font.h
        const fontInfoBuffer = Buffer.alloc(N64LibDragonFontWriter.SizeOfFontHeader);
        fontInfoBuffer.writeUInt32BE(font.size, 0);
        fontInfoBuffer.writeUInt32BE(font.lineHeight, 4);
        fontInfoBuffer.writeUInt32BE(font.glyphs.length, 8);
        fontInfoBuffer.writeUInt32BE(glyphsPerSlice, 12);

        // This needs to be kept in sync with fw64LibDragonFontGlyph in libdragon_font.h
        const glyphBuffer = Buffer.alloc(N64LibDragonFontWriter.SizeOfFontGlyph * font.glyphs.length);
        let slice = 0;
        let glyphIndex = 0;
        let bufferOffset = 0;

        for (const glyph of font.glyphs) {
            bufferOffset = glyphBuffer.writeUInt32BE(glyph.codepoint, bufferOffset);

            // TEMP - write character frame and index
            bufferOffset = glyphBuffer.writeUInt16BE(slice, bufferOffset);
            bufferOffset = glyphBuffer.writeUInt16BE(glyphIndex, bufferOffset);

            bufferOffset = glyphBuffer.writeInt8(glyph.top, bufferOffset);
            bufferOffset = glyphBuffer.writeInt8(glyph.left, bufferOffset);
            bufferOffset = glyphBuffer.writeInt8(glyph.advance, bufferOffset);
            bufferOffset = glyphBuffer.writeInt8(glyph.height, bufferOffset);

            glyphIndex += 1;
            if (glyphIndex >= glyphsPerSlice) {
                slice += 1;
                glyphIndex = 0;
            }
        }

        // Write the lookup table for texture coordinates info the buffer
        const SizeOfGlyphTexCoords = (2 * 2 /* s, t */) * 4 /* each corner */;
        const textCoordLookupBuffer = Buffer.alloc(glyphsPerSlice * SizeOfGlyphTexCoords);
        bufferOffset = 0;
        for (const value of font.texCoordLookup) {
            bufferOffset = textCoordLookupBuffer.writeInt16BE(value, bufferOffset);
        }

        // the writing order needs to be kept in sync with reading order in fw64_font_load_from_datasource in libdragon_font.c
        const file = fs.openSync(destPath, "w");
        fs.writeSync(file, fontInfoBuffer);
        imageWriter.writeImageSlicesToFile(libdragonSpritePaths, libdragonSpritePaths.length, 1, file);
        fs.writeSync(file, glyphBuffer);
        fs.writeSync(file, textCoordLookupBuffer);

        fs.closeSync(file);
    }
};

module.exports = N64LibDragonFontWriter;
