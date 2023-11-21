const ImageWriter = require("./ImageWriter");
const processImage = require("./ProcessImage");

const Material = require("../gltf/Material");

const fs = require("fs");
const path = require("path");

async function writeBundleImages(materialBundle, desktopImages, file) {
    for (const imageIndex of materialBundle.images) {
        await ImageWriter.writeToOpenStream(desktopImages[imageIndex], file);
    }
}

function writeBundleTextures(gltfData, materialBundle, file) {
    const textureBuffer = Buffer.alloc(12);

    for (const textureIndex of materialBundle.textures) {
        const texture = gltfData.textures[textureIndex];
        const bundledImageIndex = materialBundle.getBundledImageIndex(texture.image);

        let index = 0;
        index = textureBuffer.writeUInt32LE(bundledImageIndex, index);
        index = textureBuffer.writeUInt32LE(texture.wrapS, index);
        index = textureBuffer.writeUInt32LE(texture.wrapT, index);

        fs.writeSync(file, textureBuffer);
    }
}

function writeBundleMaterials(gltfData, materialBundle, file) {
    const materialBuffer = Buffer.alloc(24);

    for (const materialIndex of materialBundle.materials) {
        const material = gltfData.materials[materialIndex]
        const bundledTextureIndex = material.hasTexture() ? materialBundle.getBundledTextureIndex(material.texture) : Material.NoTexture;

        let index = 0;
        index = materialBuffer.writeUInt32LE(bundledTextureIndex, index);
        index = materialBuffer.writeUInt32LE(material.shadingMode, index);

        for (let i = 0; i < 4; i++) {
            index = materialBuffer.writeFloatLE(material.color[i] / 255.0, index);
        }
        
        fs.writeSync(file, materialBuffer);
    }
}

class MaterialBundleInfo {
    meshBundle = null;

    constructor(meshBundle) {
        this.meshBundle = meshBundle;
    }

    get buffer() {
        const buff = Buffer.alloc(12);

        let index = 0;
        index = buff.writeUInt32LE(this.meshBundle.images.length, index);
        index = buff.writeUInt32LE(this.meshBundle.textures.length, index);
        index = buff.writeUInt32LE(this.meshBundle.materials.length, index);

        return buff;
    }
}

async function write(materialBundle, desktopImages, gltfData, file) {
    const materialBundleInfo = new MaterialBundleInfo(materialBundle);
    fs.writeSync(file, materialBundleInfo.buffer);

    await writeBundleImages(materialBundle, desktopImages, file);
    writeBundleTextures(gltfData, materialBundle, file);
    writeBundleMaterials(gltfData, materialBundle, file);
}

async function createDesktopImages(gltfData) {
    const desktopImages = [];
    const gltfDir = path.dirname(gltfData.gltfPath);

    for (const imageJson of gltfData.images) {
        desktopImages.push(await processImage(imageJson, null, gltfDir, null));
    }

    return desktopImages;
}

module.exports = {
    write: write,
    createDesktopImages: createDesktopImages
};
