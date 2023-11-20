const fs = require("fs");
const N64Material = require("./N64Material");

function write(gltfData, materialBundle, n64Images, file) {
    computeN64TextureMasks(materialBundle, gltfData, n64Images);

    writeMaterialBundleInfo(materialBundle, file);

    for (const imageIndex of materialBundle.images) {
        const image = n64Images[imageIndex];
        fs.writeSync(file, image.assetBuffer);
    }

    const textureBuffer = Buffer.alloc(24);
    for (const textureIndex of materialBundle.textures) {
        const texture = gltfData.textures[textureIndex];
        const bundledImageIndex = materialBundle.getBundledImageIndex(texture.image);

        let index = 0;

        index = textureBuffer.writeUInt32BE(bundledImageIndex, index);
        index = textureBuffer.writeUInt32BE(texture.wrapS, index);
        index = textureBuffer.writeUInt32BE(texture.wrapT, index);
        index = textureBuffer.writeUInt32BE(texture.maskS, index);
        index = textureBuffer.writeUInt32BE(texture.maskT, index);
        index = textureBuffer.writeUInt32BE(texture.paletteIndex, index);
    
        fs.writeSync(file, textureBuffer);
    }

    const materialBuffer = Buffer.alloc(16)
    for (const materialIndex of materialBundle.materials) {
        const material = gltfData.materials[materialIndex];
        const bundledTextureIndex = material.hasTexture() ? materialBundle.getBundledTextureIndex(material.texture) : N64Material.NoTexture;
        
        let index = 0;

        index = materialBuffer.writeUInt8(material.color[0], index);
        index = materialBuffer.writeUInt8(material.color[1], index);
        index = materialBuffer.writeUInt8(material.color[2], index);
        index = materialBuffer.writeUInt8(material.color[3], index);
        index = materialBuffer.writeUInt32BE(bundledTextureIndex, index);
        index = materialBuffer.writeUInt32BE(material.textureFrame, index);
        index = materialBuffer.writeUInt32BE(material.shadingMode, index);

        fs.writeSync(file, materialBuffer);
    }
}

function computeN64TextureMasks(materialBundle, gltfData, n64Images) {
    for (const textureIndex of materialBundle.textures) {
        const texture = gltfData.textures[textureIndex];
        const bundledImageIndex = materialBundle.getBundledImageIndex(texture.image);

        if (bundledImageIndex < 0) {
            throw new Error(`Unable to compute N64 texture mask for image: ${texture.image}`);
        }

        const image = n64Images[bundledImageIndex];
        texture.maskS = Math.log2(image.width);
        texture.maskT = Math.log2(image.height);
    }
}

function writeMaterialBundleInfo(materialBundle, file){
    const buff = Buffer.alloc(12);

    let index = 0;
    index = buff.writeUInt32BE(materialBundle.images.length, index);
    index = buff.writeUInt32BE(materialBundle.textures.length, index);
    index = buff.writeUInt32BE(materialBundle.materials.length, index);

    fs.writeSync(file, buff);
}

module.exports = {
    write: write
};
