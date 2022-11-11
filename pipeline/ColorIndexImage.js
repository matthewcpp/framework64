const fs = require("fs");

class ColorIndexImage {
    pixels;
    palettes;
    primaryPalette;
    _primaryPaletteColorIndices;
    _colorLookup;

    width;
    height;

    createfromBuffer(buffer, width, height) {
        this.pixels = [];
        this.palettes = [];
        this.primaryPalette = [];
        this._primaryPaletteColorIndices = [];
        this._colorLookup = new Map();

        this.width = width;
        this.height = height;

        const pixelCount = width * height;

        for (let i = 0; i < pixelCount; i++) {
            const index = i * 4;
            const r = buffer[index];
            const g = buffer[index + 1];
            const b = buffer[index + 2];
            const a = buffer[index + 3];

            const colorKey = this._getColorKey(r, g, b, a);

            if (!this._colorLookup.has(colorKey)) {
                this._colorLookup.set(colorKey, this.primaryPalette.length);
                this.primaryPalette.push([r, g, b, a]);
                this._primaryPaletteColorIndices.push(i);
            }
            
            this.pixels.push(this._colorLookup.get(colorKey));
        }

        this.palettes.push(this.primaryPalette);
    }

    _getColorKey(r, g, b, a) {
        return (r << 24) | (g << 16) | (b << 8) | (a);
    }

    getColorIndex(r, g, b, a) {
        const colorKey = this._getColorKey(r, g, b, a);
        return this._colorLookup.get(colorKey);
    }
}

module.exports = ColorIndexImage;