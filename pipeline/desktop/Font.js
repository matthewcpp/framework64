const Image = require("./Image");
const Util = require("../Util");

const opentype = require("opentype.js");
const { createCanvas } = require("canvas");

const fs = require("fs");

class Font {
    _font = null;

    name = null;
    size = 0;
    glyphs = null;
    image = null;
    tileWidth = 0;
    tileHeight = 0;
    scale = 0;
    ascender = 0;

    constructor(name) {
        this.name = name;
    }

    async loadGlyphs(path, sourceString, size) {
        this._font = await opentype.load(path);
        this.size = size;

        this.scale = 1.0 / this._font.unitsPerEm * size;
        this.ascender = Math.ceil(this._font.ascender * this.scale);

        const codepoints = [...new Set(Array.from(sourceString))];
        codepoints.sort();

        this.tileWidth = 0;
        this.tileHeight = 0;
        this.glyphs = [];

        const missingCharacter = this._font.glyphs.get(0);
        this._parseGlyphData(missingCharacter, 0);

        for (const codepoint of codepoints) {
            const glyph = this._font.charToGlyph(codepoint);
            this._parseGlyphData(glyph);
        }
    }

    loadImageFontGlyphs(name, sourceString, tileWidth, tileHeight) {
        this.name = name;
        this.tileWidth = tileWidth;
        this.tileHeight = tileHeight;
        this.size = tileHeight;

        this.glyphs = [];
        for (let i = 0; i < sourceString.length; i++) {
            this.glyphs.push({
                codepoint: sourceString.charCodeAt(i),
                top: 0,
                left: 0,
                advance: tileWidth,
                height: tileHeight
            });
        }
    }

    _parseGlyphData(glyph) {
        const xMin = typeof (glyph.xMin) == "undefined" ? 0 : glyph.xMin;
        const xMax = typeof (glyph.xMin) == "undefined" ? 0 : glyph.xMax;
        const yMin = typeof (glyph.yMin) == "undefined" ? 0 : glyph.yMin;
        const yMax = typeof (glyph.yMax) == "undefined" ? 0 : glyph.yMax;

        const glyphInfo = {
            codepoint: glyph.unicodes.length > 0 ? glyph.unicodes[0] : 0,
            top: this.ascender - Math.ceil(yMax * this.scale),
            left: Math.floor(xMin * this.scale),
            advance: Math.ceil(glyph.advanceWidth * this.scale),
            height: Math.ceil((yMax - yMin) * this.scale),
            glyph: glyph
        }

        this.tileWidth = Math.max(this.tileWidth, Math.ceil((xMax - xMin) * this.scale));
        this.tileHeight = Math.max(this.tileHeight, glyphInfo.height);

        this.glyphs.push(glyphInfo);
    }

    async createGlImage() {
        this.tileWidth = Util.nextPowerOf2(this.tileWidth);
        this.tileHeight = Util.nextPowerOf2(this.tileHeight);

        const scale = 1.0 / this._font.unitsPerEm * this.size;
        const imageDimensions = this._calculateGlTextureSize(this.tileWidth, this.tileHeight, this.glyphs.length);
        const imageWidth = imageDimensions.width;
        const imageHeight = imageDimensions.height;
        const canvas = createCanvas(imageWidth, imageHeight);
        const ctx = canvas.getContext('2d');

        let x = 0, y = 0;

        for (let i = 0; i < this.glyphs.length; i++) {
            const glyphInfo = this.glyphs[i];
            const glyph = glyphInfo.glyph;
            const yMax = typeof (glyph.yMax) == "undefined" ? 0 : glyph.yMax;
            const path = glyph.getPath(x + -glyphInfo.left, y + Math.ceil(yMax* scale), this.size);

            path.fill = "white";
            path.draw(ctx);

            x += this.tileWidth;
            if (x >= imageWidth) {
                x = 0;
                y += this.tileHeight;
            }
        }

        // note this actualy dumps image in BRGA but since we write everything as 255 white no swapping is needed
        const imageBuffer = canvas.toBuffer("raw");
        const image = new Image(this.name);
        await image.loadBuffer(imageBuffer, imageWidth, imageHeight);
        image.setSliceCounts(imageWidth / this.tileWidth, imageHeight / this.tileHeight);

        this.image = image;
    }

    _calculateGlTextureSize(tileWidth, tileHeight, count) {
        const glyphArea = tileWidth * tileHeight;
        const totalNeeded = count * glyphArea;
        let totalAvailable = tileWidth * tileHeight;

        while (totalAvailable < totalNeeded) {
            if (tileWidth <= tileHeight)
                tileWidth = Util.nextPowerOf2(tileWidth + 1);
            else
                tileHeight = Util.nextPowerOf2(tileHeight +1);

            totalAvailable = tileWidth * tileHeight
        }
        return {
            width: tileWidth,
            height: tileHeight
        }
    }

    get desktopGlyphBuffer() {
        const bufferSize = this.glyphs.length * 20;
        const buffer = Buffer.allocUnsafe(bufferSize);

        let offset = 0;
        for (const glyph of this.glyphs) {
            offset = buffer.writeUInt32LE(glyph.codepoint, offset);
            offset = buffer.writeFloatLE(glyph.top, offset);
            offset = buffer.writeFloatLE(glyph.left, offset);
            offset = buffer.writeFloatLE(glyph.advance, offset);
            offset = buffer.writeFloatLE(glyph.height, offset);
        }

        return buffer;
    }
}

module.exports = Font;