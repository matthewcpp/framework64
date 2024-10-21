const processImage = require("./ProcessImage");
const Material = require("../gltf/Material");

const tmp = require("tmp");

const fs = require("fs");
const path = require("path");

// this temp function will write out all the images in this asset bundle
async function tempWriteLibdragonImages(materialBundle, dfsAssets, outputDir) {
    const gltfLoader = materialBundle.gltfData;
    const assetDirectory = path.dirname(gltfLoader.gltfPath);
    const images = [];

    dfsAssets.defineAssetsInHeader = false;
    for (const imageIndex of materialBundle.images) {
        const image = materialBundle.gltfData.images[imageIndex];
        const imageId = await processImage(image, dfsAssets, assetDirectory, outputDir);
        images.push(imageId);
    }

    dfsAssets.defineAssetsInHeader = true;

    return images;
}

function write(materialBundle, libdragonImages, gltfData, file) {
    if (libdragonImages.length != materialBundle.images.length) {
        throw new Error("material bundle image length is not equal to assetId array length");
    }

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

// Writes out the list of image asset ids to the buffer
function _writeImages(libdragonImages, file) {
    if (libdragonImages.length === 0) {
        return;
    }

    const imageIdsBuffer = Buffer.alloc(4 * libdragonImages.length);

    let index = 0;
    for (const image of libdragonImages) {
        index = imageIdsBuffer.writeUint32BE(image, index);
    }

    fs.writeSync(file, imageIdsBuffer);
}

function _writeTextures(materialBundle, gltfData, file) {
    // this buffer needs to align with n64_libdragon/texture.h
    const textureBuffer = Buffer.alloc(4);

    for (const textureIndex of materialBundle.textures) {
        const texture = gltfData.textures[textureIndex];
        const bundledImageIndex = materialBundle.getBundledImageIndex(texture.image);

        textureBuffer.writeUInt32BE(bundledImageIndex, 0);
        fs.writeSync(file, textureBuffer);
    }
}

function _writeMaterials(materialBundle, gltfData, file) {
    // this buffer needs to align with n64_libdragon/material.h
    const materialBuffer = Buffer.alloc(12);

    for (const materialIndex of materialBundle.materials) {
        const material = gltfData.materials[materialIndex];
        const bundledTextureIndex = material.hasTexture() ? materialBundle.getBundledTextureIndex(material.texture) : Material.NoTexture;

        let index = 0;
        index = materialBuffer.writeUint32BE(material.shadingMode, index);
        index = materialBuffer.writeUint32BE(bundledTextureIndex, index);
        index = materialBuffer.writeUInt8(material.color[0], index);
        index = materialBuffer.writeUInt8(material.color[1], index);
        index = materialBuffer.writeUInt8(material.color[2], index);
        index = materialBuffer.writeUInt8(material.color[3], index);

        fs.writeSync(file, materialBuffer);
    }
}

module.exports = {
    write: write,
    tempWriteLibdragonImages:tempWriteLibdragonImages
};
