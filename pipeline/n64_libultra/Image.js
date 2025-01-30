const ImageBase = require("../ImageBase");
const ColorIndexImage = require("../ColorIndexImage");

class Image extends ImageBase {
    format;
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

    constructor(name, /* Image.Format */ format) {
        super(name);

        if (format === null || format === undefined) {
            throw new Error(`Must specify format when creating Image`);
        }

        this.format = format;
    }

    assign(jimpData) {
        super.assign(jimpData);
        this.colorIndexImage = null;
    }

    createColorIndexImage() {
        this.colorIndexImage = new ColorIndexImage();
        this.colorIndexImage.createfromBuffer(this.data, this.width, this.height);
    }
}

module.exports = Image;