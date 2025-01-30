const Jimp = require("jimp");

/** Provides base routines for loading and processing images */
class ImageBase {
    name;
    _data = null;
    hslices = 1;
    vslices = 1;

    constructor(name) {
        this.name = name;
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

    loadBuffer(buffer, width, height) {
        return new Promise((resolve) => {
            return new Jimp({data: buffer, width: width, height: height}, (error, image) => {
                this._data = image;
                resolve();
            })
        });
    }

    /* Loads all the frames and blits into this image.  Also sets the slice counts. */
    async loadAtlas(frames, hslices, vslices, frameWidth, frameHeight) {
        await this.loadEmpty(hslices * frameWidth, vslices * frameHeight);

        for (let y = 0; y < vslices; y++) {
            for (let x = 0; x < hslices; x++) {
                const frame = await Jimp.read(frames[y * hslices + x]);
                this._data.blit(frame, x * frameWidth, y * frameHeight);
            }
        }

        this.setSliceCounts(hslices, vslices);
    }

    assign(jimpData) {
        this._data = jimpData;
    }

    crop(x, y, w, h) {
        this._data.crop(x, y, w, h);
    }

    resize(width, height) {
        this._data.resize(width, height);
    }

    writeToFile(path) {
        return this._data.writeAsync(path);
    }

    static ImageSlices = class {
        hslices;
        vslices;
        images = [];
    
        constructor(horizontalSlices, verticalSlices) {
            this.hslices = horizontalSlices;
            this.vslices = verticalSlices;
        }
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
        const imageSlices = new ImageBase.ImageSlices(horizontalSlices, verticalSlices);

        const sliceWidth = this.width / horizontalSlices;
        const sliceHeight = this.height / verticalSlices;

        for (let sliceY = 0; sliceY < verticalSlices; sliceY++) {
            for (let sliceX = 0; sliceX < horizontalSlices; sliceX++) {
                const originX = sliceX * sliceWidth;
                const originY = sliceY * sliceHeight;

                const currentSliceWidth = Math.min(sliceWidth, this.width - originX);
                const currentSliceHeight = Math.min(sliceHeight, this.height - originY);

                const slice = this._createSlice(originX, originY, currentSliceWidth, currentSliceHeight);
                imageSlices.images.push(slice);
            }
        }

        return imageSlices;
    }

    setSliceCounts(hslices, vslices) {
        this.hslices = hslices;
        this.vslices = vslices;
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

    get hasAlpha() {
        return this._data.hasAlpha();
    }
}

module.exports = ImageBase;