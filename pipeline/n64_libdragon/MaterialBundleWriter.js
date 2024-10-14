const ImageConvert = require("./ImageConvert");
const Material = require("../gltf/Material");

const tmp = require("tmp");

const fs = require("fs");
const path = require("path");

async function createLibdragonImages(gltfData) {
    const images = [];

    const gltfDir = path.dirname(gltfData.gltfPath);
    const tempDir = tmp.dirSync({unsafeCleanup: true});

    try {
        for (const imageJson of gltfData.images) {
            await ImageConvert.convertLibdragonSprite(imageJson.src, gltfDir, tempDir.name);
            const spriteFileName = ImageConvert.getSpriteNameForImage(imageJson.src);
            const spriteBuffer = fs.readFileSync(path.join(tempDir.name, spriteFileName));
            console.log(`Converted sprite: ${spriteFileName} (${spriteBuffer.length})`);
            images.push(spriteBuffer);
        }
    }
    finally {
        tempDir.removeCallback();
    }

    return images;
}

function write(materialBundle, libdragonImages, gltfData, file) {
    _writeMaterialBundleHeader(materialBundle, file);
    _writeImages(libdragonImages, file);
    _writeTextures(materialBundle, gltfData, file);
    _writeMaterials(materialBundle, gltfData, file);
}

/// this needs to coorespond to include/n64_libdragon/material_bundle.c
function _writeMaterialBundleHeader(materialBundle, file) {
    const buffer = Buffer.alloc(12);

    let index = 0;
    index = buffer.writeUint32BE(materialBundle.images.length, index);
    index = buffer.writeUint32BE(materialBundle.textures.length, index);
    index = buffer.writeUint32BE(materialBundle.materials.length, index);

    fs.writeSync(file, buffer);
}

function _writeImages(libdragonImages, file) {
    if (libdragonImages.length === 0) {
        return;
    }

    // this buffer needs to align with Image header n64_libdragon/image.c
    const imageSizeBuffer = Buffer.alloc(4);

    for (const image of libdragonImages) {
        imageSizeBuffer.writeUint32BE(image.length, 0);
        fs.writeSync(file, imageSizeBuffer);
        fs.writeSync(file, image);
    }
}

function _writeTextures(materialBundle, gltfData, file) {
    // this buffer needs to align with n64_libdragon/texture.h
    const textureBuffer = Buffer.alloc(4);

    for (const textureIndex of materialBundle.textures) {
        const texture = gltfData.textures[textureIndex];
        const bundledImageIndex = materialBundle.getBundledImageIndex(texture.image);

        textureBuffer.writeUInt32LE(bundledImageIndex, 0);
        fs.writeSync(file, textureBuffer);
    }
}

function _writeMaterials(materialBundle, gltfData, file) {
    // this buffer needs to align with n64_libdragon/material.h
    const materialBuffer = Buffer.alloc(24);

    for (const materialIndex of materialBundle.materials) {
        const material = gltfData.materials[materialIndex];
        const bundledTextureIndex = material.hasTexture() ? materialBundle.getBundledTextureIndex(material.texture) : Material.NoTexture;

        let index = 0;
        index = materialBuffer.writeUint32BE(material.shadingMode, index);
        index = materialBuffer.writeUint32BE(bundledTextureIndex, index);
        index = materialBuffer.writeFloatBE(material.color[0] / 255.0, index);
        index = materialBuffer.writeFloatBE(material.color[1] / 255.0, index);
        index = materialBuffer.writeFloatBE(material.color[2] / 255.0, index);
        index = materialBuffer.writeFloatBE(material.color[3] / 255.0, index);

        fs.writeSync(file, materialBuffer);
    }
}

module.exports = {
    createLibdragonImages: createLibdragonImages,
    write: write
};
