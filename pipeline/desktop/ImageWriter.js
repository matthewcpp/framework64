const fs = require("fs");

class DesktopImageWriter {
    async writeFile(image, destPath) {
        const imageFile = fs.openSync(destPath, "w");
        await this.writeToOpenStream(image, imageFile);
        fs.closeSync(imageFile);
    }

    async writeToOpenStream(image, fileStream) {
        const primaryImageFileBuffer = await image.getPrimaryFileBuffer();

        const imageHeader = new ImageHeader(image, primaryImageFileBuffer.length);
        fs.writeSync(fileStream, imageHeader.buffer);
        fs.writeSync(fileStream, primaryImageFileBuffer);

        if (image.additionalPalettes.length == 0) {
            return;
        }

        const fileSizesBuffer = Buffer.alloc(image.additionalPalettes.length * 4);
        let index = 0;
        for (const additionalPalette of image.additionalPalettes) {
            index = fileSizesBuffer.writeUint32LE(additionalPalette.length, index);
        }

        fs.writeSync(fileStream, fileSizesBuffer);

        for (const additionalPalette of image.additionalPalettes) {
            fs.writeSync(fileStream, additionalPalette);
        }
    }
};

// this class should correspond to ImageHeader in desktop/image.cpp
class ImageHeader {
    static BufferSize = 20;

    buffer = Buffer.alloc(ImageHeader.BufferSize);

    constructor(image, primaryImageDataSize) {
        this.assign(image, primaryImageDataSize);
    }

    assign(image, primaryImageDataSize) {
        let index = 0;
        index = this.buffer.writeUint32LE(image.hslices, index);
        index = this.buffer.writeUint32LE(image.vslices, index);
        index = this.buffer.writeUint32LE(image.isIndexed ? 1 : 0, index);
        index = this.buffer.writeUint32LE(primaryImageDataSize, index);
        index = this.buffer.writeUint32LE(image.additionalPalettes.length, index);
    }
};

module.exports = DesktopImageWriter;
