const Jimp = require("jimp");
const path = require("path");

class Image {
    name;
    hslices = 1;
    vslices = 1;
    isIndexed = false;
    additionalPalettes = [];

    constructor(name) {
        this.name = name;
        this._data = null;
    }

    async load(path) {
        this._data = await Jimp.read(path);
    }

    loadEmpty(width, height) {
        return new Promise((resolve) => {
            new Jimp(width, height, (error, image) => {
                this._data = image;
                resolve();
            })
        })
    }

    async loadAtlas(frames, hslices, vslices, frameWidth, frameHeight) {
        await this.loadEmpty(hslices * frameWidth, vslices * frameHeight);

        for (let y = 0; y < vslices; y++) {
            for (let x = 0; x < hslices; x++) {
                const frame = await Jimp.read(frames[y * hslices + x]);
                this._data.blit(frame, x * frameWidth, y * frameHeight);
            }
        }
    }

    loadBuffer(buffer, width, height) {
        return new Promise((resolve) => {
            return new Jimp({data: buffer, width: width, height: height}, (error, image) => {
                this._data = image;
                resolve();
            })
        });
    }

    assign(jimpData) {
        this._data = jimpData;
    }

    crop(x, y, w, h) {
        this._data.crop(x, y, w, h);
    }

    async writeToFile(path) {
        await this._data.writeAsync(path);
    }

    async getPrimaryFileBuffer() {
        return await this._data.getBufferAsync(Jimp.MIME_PNG);
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

    resize(width, height) {
        this._data.resize(width, height);
    }

    setSliceCounts(hslices, vslices) {
        this.hslices = hslices;
        this.vslices = vslices;
    }
}

module.exports = Image;