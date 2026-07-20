const FontBase = require("../FontBase");
const Image = require("../ImageBase");

const { createCanvas } = require("canvas");

class N64LibDragonFont extends FontBase {
    constructor(name) {
        super(name);
    }

    static MaxImageWidth = 64;
    static MaxImageHeight = 64;

    /** Generates Images for all the images which contain the glyphs for the characters in this font */
    async createGlyphImages() {
        const glyphImages = [];
        const scale = 1.0 / this._fontFile.unitsPerEm * this.size;

        const horizontalGlyphCount = N64LibDragonFont.MaxImageWidth /this.tileWidth;
        const verticalGlyphCount = N64LibDragonFont.MaxImageHeight / this.tileHeight;
        const glyphsPerTile = horizontalGlyphCount * verticalGlyphCount;
        
        const imageWidth = N64LibDragonFont.MaxImageWidth;
        const imageHeight = N64LibDragonFont.MaxImageHeight;

        let canvas = createCanvas(imageWidth, imageHeight);
        let ctx = canvas.getContext('2d');

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

                if (y >= imageHeight) {
                    y = 0;
                    glyphImages.push(await this._createSliceImage(canvas, imageWidth, imageHeight));
                    canvas = createCanvas(imageWidth, imageHeight);
                    ctx = canvas.getContext('2d');
                }
            }
        }

        // if there are any glyphs left in the current image then generate the image for them
        if (x !== 0 || y !== 0) {
            glyphImages.push(await this._createSliceImage(canvas, imageWidth, imageHeight));
        }

        return glyphImages;
    }

    async _createSliceImage(canvas, imageWidth, imageHeight) {
        // note this actualy dumps image in BRGA but since we write everything as 255 white no swapping is needed
        const imageBuffer = canvas.toBuffer("raw");
        const image = new Image(this.name);
        await image.loadBuffer(imageBuffer, imageWidth, imageHeight);
        N64LibDragonFont.undoPreMultipliedAlpha(image);
        return image;
    }

    static undoPreMultipliedAlpha(image) {
        const pixelCount = image.width * image.height;

        for (let i = 0; i < pixelCount; i++) {
            const index = i * 4;
            image.data[index] = 255;
            image.data[index + 1] = 255;
            image.data[index + 2] = 255;
        }
    }
};

module.exports = N64LibDragonFont;