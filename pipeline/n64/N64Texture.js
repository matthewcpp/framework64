class N64Texture {
    static WrapMode = {
        Repeat: 0,
        Mirror: 1,
        Clamp: 2
    };

    image;
    wrapS = N64Texture.WrapMode.Repeat;
    wrapT = N64Texture.WrapMode.Repeat;
    maskS = 0;
    maskT = 0;
    paletteIndex = 0;

    constructor(image) {
        this.image = image;
    }

    get buffer() {
        const buff = Buffer.alloc(24);
        let index = 0;

        index = buff.writeUInt32BE(this.image, index);
        index = buff.writeUInt32BE(this.wrapS, index);
        index = buff.writeUInt32BE(this.wrapT, index);
        index = buff.writeUInt32BE(this.maskS, index);
        index = buff.writeUInt32BE(this.maskT, index);
        index = buff.writeUInt32BE(this.paletteIndex, index);

        return buff;
    }
}

module.exports = N64Texture;