class Texture {
    /** These values need to be kept in sync with desktop/material_bundle.cpp */
    static WrapMode = {
        Repeat: 0,
        Mirror: 1,
        Clamp: 2
    };

    /** index into GLTF data `images` array */
    image;
    wrapS = Texture.WrapMode.Repeat;
    wrapT = Texture.WrapMode.Repeat;

    /** N64 Specific */
    maskS = 0;

    /** N64 Specific */
    maskT = 0;
    paletteIndex = 0;

    constructor(image) {
        this.image = image;
    }
}

module.exports = Texture;