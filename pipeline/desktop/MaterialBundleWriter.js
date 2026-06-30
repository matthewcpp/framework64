const DesktopImageWriter = require("./ImageWriter");
const DesktopImageProcessor = require("./ImageProcessor");

const Material = require("../gltf/Material");

const fs = require("fs");
const path = require("path");

// Note: the write order of the objects in this file needs to match up with the reading order in src/framework64/desktop/material_bundle.cpp
class DesktopMaterialBundleWriter {
    _environment;
    _imageProcessor;

    constructor(environment, imageProcessor) {
        this._environment = environment;
        this._imageProcessor = imageProcessor;
    }

    async write(materialBundle, desktopImages, gltfData, file) {
        const materialBundleInfo = new MaterialBundleInfo(materialBundle);
        fs.writeSync(file, materialBundleInfo.buffer);

        await this._writeBundleImages(materialBundle, desktopImages, file);
        this._writeBundleTextures(gltfData, materialBundle, file);
        this._writeBundleMaterials(gltfData, materialBundle, file);
    }

    async createImages(gltfData) {
        if (gltfData.images.length === 0) {
            return [];
        }

        const desktopImages = [];
        const gltfDir = path.dirname(gltfData.gltfPath);

        for (const imageJson of gltfData.images) {
            desktopImages.push(await this._imageProcessor.convertWithoutBundling(imageJson, gltfDir));
        }

        return desktopImages;
    }

    async _writeBundleImages(materialBundle, desktopImages, file) {
        for (const imageIndex of materialBundle.images) {
            await new DesktopImageWriter().writeToOpenStream(desktopImages[imageIndex], file);
        }
    }

    _writeBundleTextures(gltfData, materialBundle, file) {
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

    _writeBundleMaterials(gltfData, materialBundle, file) {
        const materialBuffer = Buffer.alloc(28);

        for (const materialIndex of materialBundle.materials) {
            const material = gltfData.materials[materialIndex]
            const bundledTextureIndex = material.hasTexture() ? materialBundle.getBundledTextureIndex(material.texture) : Material.NoTexture;

            let index = 0;
            index = materialBuffer.writeUInt32LE(bundledTextureIndex, index);
            index = materialBuffer.writeUInt32LE(material.shadingMode, index);
            index = materialBuffer.writeUInt32LE(material.textureFrame, index);

            for (let i = 0; i < 4; i++) {
                index = materialBuffer.writeFloatLE(material.color[i] / 255.0, index);
            }
            
            fs.writeSync(file, materialBuffer);
        }
    }
};

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

module.exports = DesktopMaterialBundleWriter;
