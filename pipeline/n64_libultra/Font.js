const FontBase = require("../FontBase");
const Image = require("./Image")

const { createCanvas } = require("canvas");

class Font extends FontBase{
    constructor(name) {
        super(name);
    }

    async createFontImage(imageFormat) {
        const imageWidth = this.tileWidth;
        const imageHeight = this.glyphs.length * this.tileHeight;
        const canvas = createCanvas(this.tileWidth, imageHeight);
        const ctx = canvas.getContext("2d");

        for (let i = 0; i < this.glyphs.length; i++) {
            const glyphInfo = this.glyphs[i];
            const glyph = glyphInfo.opentypeGlyph;
            const yMax = typeof (glyph.yMax) == "undefined" ? 0 : glyph.yMax;
            const path = glyph.getPath(-glyphInfo.left, (i * this.tileHeight) + Math.ceil(yMax* this.scale), this.size);

            path.fill = "white";
            path.draw(ctx);
        }

        // note this actualy dumps image in BRGA but since we write everything as 255 white no swapping is needed
        const imageBuffer = canvas.toBuffer("raw");
        const image = new Image(this.name, imageFormat); // TODO: This can probably change to use IA Tex
        await image.loadBuffer(imageBuffer, imageWidth, imageHeight);
        this.undoPreMultipliedAlpha(image);

        return image;
    }

    undoPreMultipliedAlpha(image) {
        const pixelCount = image.width * image.height;

        for (let i = 0; i < pixelCount; i++) {
            const index = i * 4;
            image.data[index] = 255;
            image.data[index + 1] = 255;
            image.data[index + 2] = 255;
        }
    }
}

module.exports = Font;