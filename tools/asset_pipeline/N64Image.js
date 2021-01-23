const Jimp = require("jimp");

class N64Image {
    constructor(name) {
        this.name = name;
        this._data = null;
    }

    async load(path) {
        this._data = await Jimp.read(path);
    }

    get width() {
        return this._data.bitmap.width;
    }

    get height() {
        return this._data.bitmap.height;
    }

    get data() {
        return this._data.bitmap.data;
    }

    static encode16bpp(data, width, height) {
        const pixelCount = width * height;

        // rgba data (2 bytes per pixel)
        const bufferSize = pixelCount * 2;

        const buffer = Buffer.alloc(bufferSize);
        let offset = 0;

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

    resize(width, height) {
        this._data.resize(width, height);
    }

    _createSlice(originX, originY, sliceWidth, sliceHeight) {
        const data = this._data.bitmap.data;
        const slice = [];

        for (let y = 0; y < sliceHeight; y++) {
            for (let x = 0; x < sliceWidth; x++) {
                const index = ((originY + y) * this.width + (originX + x)) * 4;
                slice.push(data[index], data[index + 1], data[index+2], data[index + 3]);
            }
        }

        return slice;
    }

    slice(horizontalSlices, verticalSlices) {
        let slices = [];

        const sliceWidth = this.width / horizontalSlices;
        const sliceHeight = this.height / verticalSlices;

        for (let sliceY = 0; sliceY < verticalSlices; sliceY++) {
            for (let sliceX = 0; sliceX < horizontalSlices; sliceX++) {
                const originX = sliceX * sliceWidth;
                const originY = sliceY * sliceHeight;

                const currentSliceWidth = Math.min(sliceWidth, this.width - originX);
                const currentSliceHeight = Math.min(sliceHeight, this.height - originY);

                const slice = this._createSlice(originX, originY, currentSliceWidth, currentSliceHeight);
                slices.push(slice);
            }
        }

        return {
            hslices: horizontalSlices,
            vslices: verticalSlices,
            images: slices
        };
    }
}

module.exports = N64Image;