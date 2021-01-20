const Jimp = require("jimp");

class N64Image {
    constructor(name) {
        this.name = name;
        this.data = null;
        this.hSlices = 1;
        this.vSlices = 1;
    }

    async load(path) {
        this.data = await Jimp.read(path);
    }

    get width() {
        return this.data.bitmap.width;
    }

    get height() {
        return this.data.bitmap.height;
    }
    encode16bpp() {
        const pixelCount = this.width * this.height;

        // rgba data (2 bytes per pixel)
        const bufferSize = pixelCount * 2;

        const buffer = Buffer.alloc(bufferSize);
        let offset = 0;
        const data = this.data.bitmap.data;

        for (let i = 0; i < pixelCount; i++) {
            let index = i * 4;

            // note the pixel data format is 5r 5g 5b 1a
            const r = data[index] >> 3;
            const g = data[index + 1] >> 3;
            const b = data[index + 2] >> 3;
            const a = data[index + 3] === 0 ? 0 : 1;

            const val = r << 11 | g << 6 | b << 1 | a;
            offset = buffer.writeUInt16BE(val, offset);
        }

        return buffer;
    }
}

module.exports = N64Image;