const Jimp = require("jimp");
const ColorIndexImage = require("../ColorIndexImage");

class N64Image {
    name;
    format;
    _data = null;
    colorIndexImage = null;

    /** This should align with include/n64/image.h */
    static Format = {
        RGBA16: 0,
        RGBA32: 1,
        IA8: 2,
        IA4: 3,
        I8: 4,
        I4: 5,
        CI8: 6,
        CI4: 7,
    };

    constructor(name, format) {
        this.name = name;

        if (typeof(format) === "string") {
            format = N64Image.Format[format.toUpperCase()];
        }

        if (format === null || format === undefined) {
            throw new Error(`Invalid Image format: ${format}`);
        }

        this.format = format;
    }

    async load(path) {
        this._data = await Jimp.read(path);
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
        this.colorIndexImage = null;
    }

    createColorIndexImage() {
        this.colorIndexImage = new ColorIndexImage();
        this.colorIndexImage.createfromBuffer(this.data, this.width, this.height);
    }

    crop(x, y, w, h) {
        this._data.crop(x, y, w, h);
    }

    writeToFile(path) {
        return this._data.writeAsync(path);
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