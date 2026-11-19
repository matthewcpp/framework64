const FontBase = require("../FontBase");
const Image = require("../ImageBase");

const { createCanvas } = require("canvas");

class N64LibDragonFont extends FontBase {
    constructor(name) {
        super(name);
    }

    static MaxImageWidth = 64;
    static MaxImageHeight = 64;
    horizontalGlyphCount = 0;
    verticalGlyphCount = 0;
    texCoordLookup = [];

    /** Builds the lookup table for the runtime spritebatch to properly set the texture
     * coordinates for a glyph given its index
     */
    _buildLookupTable() {
        const glyphIndexCount = this.horizontalGlyphCount * this.verticalGlyphCount;
        const horizontalStep = 1.0 / this.horizontalGlyphCount;
        const verticalStep = 1.0 / this.verticalGlyphCount;
        for (let i = 0; i < glyphIndexCount; i++) {
            let baseX = (i % this.horizontalGlyphCount) * horizontalStep;
            let baseY = Math.floor(i / this.verticalGlyphCount) * verticalStep;

            this.texCoordLookup.push(baseX + horizontalStep, baseY + verticalStep) // 1, 1
            this.texCoordLookup.push(baseX, baseY + verticalStep) // 0, 1
            this.texCoordLookup.push(baseX, baseY) // 0, 0
            this.texCoordLookup.push(baseX + horizontalStep, baseY) // 1, 0
        }

        // note: tex coords are stored in S10.5 format.
        // TODO: can we change this to be more precise?
        this.texCoordLookup = this.texCoordLookup.map((val) => {
            return val * (1 << 5);
        });
    }

    /** Generates Images for all the images which contain the glyphs for the characters in this font */
    async createGlyphImages() {
        const glyphImages = [];
        const scale = 1.0 / this._fontFile.unitsPerEm * this.size;

        this.horizontalGlyphCount = N64LibDragonFont.MaxImageWidth /this.tileWidth;
        this.verticalGlyphCount = N64LibDragonFont.MaxImageHeight / this.tileHeight;
        
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

        this._buildLookupTable();

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