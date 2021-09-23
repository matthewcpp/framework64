const N64Image = require("./N64Image")
const Util = require("../Util")

const opentype = require("opentype.js");
const { createCanvas } = require("canvas");

const fs = require("fs");
const path = require("path");

class N64Font {
    name;
    _font = null;

    constructor(name) {
        this.name = name;
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
        const codepoints = [...new Set(Array.from(sourceString))];
        codepoints.sort();

        const ascender = Math.ceil(this._font.ascender * scale);

        const glyphData = [];

        let tileWidth = 0;
        let tileHeight = 0;

        for (const codepoint of codepoints) {
            const glyph = this._font.charToGlyph(codepoint);

            const xMin = typeof (glyph.xMin) == "undefined" ? 0 : glyph.xMin;
            const xMax = typeof (glyph.xMin) == "undefined" ? 0 : glyph.xMax;
            const yMin = typeof (glyph.yMin) == "undefined" ? 0 : glyph.yMin;
            const yMax = typeof (glyph.yMax) == "undefined" ? 0 : glyph.yMax;

            const glyphInfo = {
                codepoint: codepoint.charCodeAt(0),
                top: ascender - Math.ceil(yMax * scale),
                left: Math.floor(xMin * scale),
                advance: Math.ceil(glyph.advanceWidth * scale),
                height: Math.ceil((yMax - yMin) * scale),
            }

            tileWidth = Math.max(tileWidth, Math.ceil((xMax - xMin) * scale));
            tileHeight = Math.max(tileHeight, glyphInfo.height);

            glyphData.push(glyphInfo);
        }

        tileWidth = Util.nextPowerOf2(tileWidth);
        tileHeight = Util.nextPowerOf2(tileHeight);

        const imageWidth = tileWidth;
        const imageHeight = codepoints.length * tileHeight;
        const canvas = createCanvas(tileWidth, imageHeight);
        const ctx = canvas.getContext('2d');

        for (let i = 0; i < glyphData.length; i++) {
            const glyphInfo = glyphData[i];
            const glyph = this._font.charToGlyph(String.fromCharCode(glyphInfo.codepoint));
            const yMax = typeof (glyph.yMax) == "undefined" ? 0 : glyph.yMax;
            const path = glyph.getPath(-glyphInfo.left, (i * tileHeight) + Math.ceil(yMax* scale), size);

            path.fill = "white";
            path.draw(ctx);
        }

        // note this actualy dumps image in BRGA but since we write everything as 255 white no swapping is needed
        const imageBuffer = canvas.toBuffer("raw");
        const image = new N64Image(this.name);
        await image.loadBuffer(imageBuffer, imageWidth, imageHeight);

        return  {
            name: this.name,
            size: size,
            glyphs: glyphData,
            image: image,
            tileWidth: tileWidth,
            tileHeight: tileHeight
        };
    }
}

module.exports = N64Font;