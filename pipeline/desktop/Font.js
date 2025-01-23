const FontBase = require("../FontBase");

const Image = require("./Image");
const Util = require("../Util");

const { createCanvas } = require("canvas");

class Font extends FontBase{
    size = 0;
    glyphs = null;
    image = null;


    constructor(name) {
        super(name)
    }

    async createGlImage() {
        const scale = 1.0 / this._fontFile.unitsPerEm * this.size;
        const imageDimensions = this._calculateGlTextureSize(this.tileWidth, this.tileHeight, this.glyphs.length);
        const imageWidth = imageDimensions.width;
        const imageHeight = imageDimensions.height;
        const canvas = createCanvas(imageWidth, imageHeight);
        const ctx = canvas.getContext('2d');

        let x = 0, y = 0;

        for (let i = 0; i < this.glyphs.length; i++) {
            const glyphInfo = this.glyphs[i];
            const glyph = glyphInfo.opentypeGlyph;
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