const Util = require("./Util")

const opentype = require("opentype.js");

/** Generic base class which platform asset paths can use in order to load font glyph data gicen a size and source string */
class FontBase {
    name = null;
    _fontFile = null;

    size = 0;
    scale = 0;
    ascender = 0;
    descender = 0;
    lineHeight = 0;

    tileWidth = 0;
    tileHeight = 0;

    glyphs = null;

    /** Generic glyph info class which holds codepoint metrics*/
    static GlyphInfo = class {
        codepoint;
        top;
        left;
        advance;
        height;
        opentypeGlyph;
    
        constructor(codepoint, top, left, advance, height, opentypeGlyph) {
            this.codepoint = codepoint;
            this.top = top;
            this.left = left;
            this.advance = advance;
            this.height = height;
            this.opentypeGlyph = opentypeGlyph;
        }
    }

    constructor(name) {
        this.name = name;
    }

    async load(path) {
        this._fontFile = await opentype.load(path);
    }

    async loadGlyphs(path, sourceString, size) {
        this._fontFile = await opentype.load(path);
        this.size = size;

        this.scale = 1.0 / this._fontFile.unitsPerEm * size;
        this.ascender = Math.ceil(this._fontFile.ascender * this.scale);
        this.descender = Math.ceil(this._fontFile.descender * this.scale);
        this.lineHeight = this.ascender + Math.abs(this.descender);

        this.tileWidth = 0;
        this.tileHeight = 0;

        /** contains all unique glyphs that were parsed from a source string while loading the font */
        this.glyphs = [];

        const missingCharacter = this._fontFile.glyphs.get(0);
        this._parseGlyphData(missingCharacter, 0);

        const codepoints = [...new Set(Array.from(sourceString))];
        codepoints.sort();

        for (const codepoint of codepoints) {
            const opentypeGlyph = this._fontFile.charToGlyph(codepoint);
            this._parseGlyphData(opentypeGlyph);
        }

        this.tileWidth = Util.nextPowerOf2(this.tileWidth);
        this.tileHeight = Util.nextPowerOf2(this.tileHeight);
    }

    _parseGlyphData(opentypeGlyph) {
        const xMin = typeof (opentypeGlyph.xMin) == "undefined" ? 0 : opentypeGlyph.xMin;
        const xMax = typeof (opentypeGlyph.xMin) == "undefined" ? 0 : opentypeGlyph.xMax;
        const yMin = typeof (opentypeGlyph.yMin) == "undefined" ? 0 : opentypeGlyph.yMin;
        const yMax = typeof (opentypeGlyph.yMax) == "undefined" ? 0 : opentypeGlyph.yMax;

        const glyphInfo = new FontBase.GlyphInfo(
            /* codepoint */      opentypeGlyph.unicodes.length > 0 ? opentypeGlyph.unicodes[0] : 0,
            /* top */            this.ascender - Math.ceil(yMax * this.scale),
            /* left */           Math.floor(xMin * this.scale),
            /* advance */        Math.ceil(opentypeGlyph.advanceWidth * this.scale),
            /* height */         Math.ceil((yMax - yMin) * this.scale),
            /* opentypeGlyph */  opentypeGlyph
        );

        this.tileWidth = Math.max(this.tileWidth, Math.ceil((xMax - xMin) * this.scale));
        this.tileHeight = Math.max(this.tileHeight, glyphInfo.height);

        this.glyphs.push(glyphInfo);
    }

    loadImageFontGlyphs(name, sourceString, tileWidth, tileHeight) {
        this.name = name;
        this.tileWidth = tileWidth;
        this.tileHeight = tileHeight;
        this.size = tileHeight;
        this.lineHeight = tileHeight;

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
}

module.exports = FontBase;