const Util = require("../Util");

const fs = require("fs");
const path = require("path");

const N64LibDragonImageWriter = require("./ImageWriter");

const Jimp = require("jimp");
const tmp = require("tmp");

class N64LibDragonImageProcessor {
    _environment;

    constructor(environment) {
        this._environment = environment;
    }

    async process(imageJson) {
        return this._process(imageJson, true, this._environment.assetDirectory);
    }

    /** Used by other processers to convert images without adding them into the asset bundle
     * For example, fonts and material bundles.  Note the asset directory is passed in here and
     * we are not necessarily using the environment's asset directory.
    */
    async convertWithoutBundling(imageJson, assetDirectory) {
        return this._process(imageJson, false, assetDirectory);
    }

    async _process(imageJson, bundleAssets, assetDirectory) {
        const defaultArgs = {
            hslices: 1,
            vslices: 1
        };
        imageJson = Object.assign(defaultArgs, imageJson);

        if (imageJson.hslices == 1 && imageJson.vslices == 1) {
            return this._processImage(imageJson, bundleAssets, assetDirectory);
        } else {
            return this._processSprite(imageJson, bundleAssets, assetDirectory);
        }
    }

    async _processImage(imageJson, bundleAssets, assetDirectory) {
        const name = Util.safeDefineName(Object.hasOwn(imageJson, "name") ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src)));
        const format = Object.hasOwn(imageJson, "format") ? imageJson.format.toUpperCase() : "RGBA16";
        const sourcePath = path.join(assetDirectory, imageJson.src);

        const imageWriter = new N64LibDragonImageWriter();
        const convertedSpriteFile = await imageWriter.writeLibdragonSprite(sourcePath, format);

        // write out the converted asset to its position in the asset bundle.  This appends some fw64 metadata to the image
        const spriteAssetFile = path.join(this._environment.outputDirectory, path.basename(convertedSpriteFile));
        imageWriter.writeImageToPath(convertedSpriteFile, spriteAssetFile);

        // TODO: need to make a struct here that has the id and path returned...
        if (bundleAssets) {
            return this._environment.assetBundle.addImage(spriteAssetFile, name);
        } else {
            return spriteAssetFile;
        }
    }

    async _processSprite(imageJson, bundleAssets, assetDirectory) {
        const name = Util.safeDefineName(Object.hasOwn(imageJson, "name") ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src)));
        const format = Object.hasOwn(imageJson, "format") ? imageJson.format.toUpperCase() : "RGBA16";
        const sourcePath = path.join(assetDirectory, imageJson.src);
        
        // since we use libdragon's OpenGL-like api for sprite rendering we need to break the sprites up into individual textures
        const image = new LibDragonImage();
        const slicePaths = await image.writeSpriteSlices(sourcePath, imageJson.hslices, imageJson.vslices, this._environment.outputDirectory);

        // We now have all the slices of the image written out to disk.  We need to convert them into libdragon sprites.
        const libdragonSprites = [];
        const imageWriter = new N64LibDragonImageWriter();
        for (const slicePath of slicePaths) {
            libdragonSprites.push(await imageWriter.writeLibdragonSprite(slicePath, format))
        }

        // Finally we want to combine them into the single sprite file
        const spriteAssetFile = path.join(this._environment.outputDirectory, path.basename(sourcePath, path.extname(sourcePath)) + ".sprite");
        imageWriter.writeImageSlicesToPath(libdragonSprites, imageJson.hslices, imageJson.vslices, spriteAssetFile);

        // TODO: need to make a struct here that has the id and path returned...
        if (bundleAssets) {
            return this._environment.assetBundle.addImage(spriteAssetFile, name);
        } else {
            return spriteAssetFile;
        }
        
    }
}

const ImageBase = require("../ImageBase");

class LibDragonImage extends ImageBase {
    constructor() {
        super("")
    }

    async writeSpriteSlices(spritePath, hslices, vslices, outputDirectory) {
        await this.load(spritePath);
        const slices = await this.slice(hslices, vslices);

        const sliceWidth = this._data.bitmap.width / slices.hslices;
        const sliceHeight = this._data.bitmap.height / slices.vslices;

        const slicePaths = [];

        let sliceIndex = 0;
        for (const slice of slices.images) {
            const sliceImage = new ImageBase("");
            await sliceImage.loadBuffer(new Uint8Array(slice), sliceWidth, sliceHeight);
    
            const slicePath = path.join(outputDirectory, `slice_${sliceIndex}.png`);
            await sliceImage.writeToFile(slicePath);

            slicePaths.push(slicePath);
            sliceIndex += 1;
        }

        return slicePaths;
    }
};


module.exports = N64LibDragonImageProcessor;