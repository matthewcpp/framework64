const N64Image = require("./N64Image")
const opentype = require("opentype.js");
const { createCanvas } = require("canvas")

const fs = require("fs");
const path = require("path");

const spritefontSpacer = 1;

class N64Font {
    constructor(name) {
        this.name = name;
        this._font = null;
    }

    async load(path) {
        this._font = await opentype.load(path);
    }

    async outputToDirectory(outDir, sourceString, size) {
        const data = await this.generateSpriteFont(sourceString, size);

        const imagePath = path.join(outDir, `${this.name}.png`);
        data.image.writeToFile(imagePath);

        const glyphPath = path.join(outDir, `${this.name}.json`);


        fs.writeFileSync(glyphPath, JSON.stringify({
            name: data.name,
            size: data.size,
            image: path.basename(imagePath),
            glyphs: data.glyphs
        }, null, 2));
    }

    async generateSpriteFont(sourceString, size) {
        const scale = 1.0 / this._font.unitsPerEm * size;
        const ascender = Math.round(this._font.ascender * scale)
        const descender = Math.round(this._font.descender * scale)
        const codepoints = [...new Set(Array.from(sourceString))];
        codepoints.sort();

        const glyphData = [];

        const imageWidth = 200;
        const imageHeight = 200;
        const canvas = createCanvas(imageWidth, imageHeight);
        const ctx = canvas.getContext('2d');

        let x = 0;
        let y = 0;

        let contentWidth = 0;

        let rowHeight = 0;

        for (const codepoint of codepoints) {
            const glyph = this._font.charToGlyph(codepoint);

            const glyphInfo = {
                codepoint: codepoint.charCodeAt(0),
                top: Math.ceil((this._font.ascender - glyph.yMax) * scale),
                //top: Math.ceil(size - (glyph.yMax * scale) + descender ),
                left: Math.ceil(glyph.xMin * scale),
                advance: Math.ceil(glyph.advanceWidth * scale),
                width: Math.ceil((glyph.xMax - glyph.xMin) * scale),
                height: Math.ceil((glyph.yMax - glyph.yMin) * scale),
            }

            if (x + glyphInfo.width > imageWidth) {
                y += rowHeight + spritefontSpacer;
                x = 0;
                rowHeight = 0;
            }

            glyphInfo.x = x;
            glyphInfo.y = y;

            rowHeight = Math.max(rowHeight, glyphInfo.height);

            const path = glyph.getPath(x - glyphInfo.left, y + glyphInfo.height + glyph.yMin * scale, size);
            path.fill = "white";
            path.draw(ctx);
            x += glyphInfo.width + spritefontSpacer;

            contentWidth = Math.max(contentWidth, x);

            glyphData.push(glyphInfo);
        }

        // note this actualy dumps image in BRGA but since we write everything as 255 white no swapping is needed
        const imageBuffer = canvas.toBuffer("raw");
        const image = new N64Image(this.name);
        await image.loadBuffer(imageBuffer, imageWidth, imageHeight);
        image.crop(0,0, contentWidth, y + rowHeight + spritefontSpacer);

        return  {
            name: this.name,
            size: size,
            glyphs: glyphData,
            image: image
        };
    }
}

module.exports = N64Font;