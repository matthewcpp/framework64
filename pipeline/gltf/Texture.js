class Texture {
    static WrapMode = {
        Repeat: 0,
        Mirror: 1,
        Clamp: 2
    };

    // index into GLTF data `images` array
    image;
    wrapS = Texture.WrapMode.Repeat;
    wrapT = Texture.WrapMode.Repeat;
    maskS = 0;
    maskT = 0;
    paletteIndex = 0;

    constructor(image) {
        this.image = image;
    }
}

module.exports = Texture;