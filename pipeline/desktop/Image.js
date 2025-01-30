const ImageBase = require("../ImageBase");

const Jimp = require("jimp");

class Image extends ImageBase {
    isIndexed = false;
    additionalPalettes = [];

    constructor(name, jimpImage = null) {
        super(name);
        this._data = jimpImage;
    }

    async getPrimaryFileBuffer() {
        return await this._data.getBufferAsync(Jimp.MIME_PNG);
    }
}

module.exports = Image;