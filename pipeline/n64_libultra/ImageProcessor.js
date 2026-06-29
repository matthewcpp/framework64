const Image = require("./Image")
const N64LibUltraImageWriter = require("./ImageWriter");
const ImageAtlasDefines = require("../ImageAtlasDefines");
const ProcessImageUtils = require("../ProcessImageUtils");

const Jimp = require("jimp");

const fs = require("fs");
const path = require("path");

class N64LibUltraImageProcessor {
    _environment;

    constructor(environment) {
        this._environment = environment;
    }

    /** processes an image and adds it into the asset bundle. */
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
        if (Object.hasOwn(imageJson, "src")) {
            return this._convertSprite(imageJson, bundleAssets, assetDirectory);
        } else if (Object.hasOwn(imageJson, "frames") || Object.hasOwn(imageJson, "frameDir")){
            return this._assembleSpriteAtlas(imageJson, bundleAssets, assetDirectory);
        }
    }

    async _convertSprite(imageJson, bundleAssets, assetDirectory) {
        const imagePath = path.join(assetDirectory, imageJson.src);

        const options = {
            hslices: 1,
            vslices: 1,
            format: "RGBA16"
        }
        Object.assign(options, imageJson);

        const name = Object.hasOwn(imageJson, "name") ? imageJson.name : path.basename(imagePath, path.extname(imagePath));

        const image = new Image(name, Image.Format[options.format.toUpperCase()]);
        await image.load(imagePath);

        return this._finalizeImage(image, options, assetDirectory, bundleAssets);
    }

    async _assembleSpriteAtlas(imageJson, bundleAssets, assetDirectory) {
        if (!Object.hasOwn(imageJson, "name")) {
            throw new Error("Sprite Atlas elements must specify a name");
        }

        const options = {
            format: "RGBA16"
        }

        Object.assign(options, imageJson);

        const image = new Image(imageJson.name, Image.Format[options.format.toUpperCase()]);

        const framePaths = ProcessImageUtils.getFramePathArray(imageJson, assetDirectory);
        const frameSize = imageJson.frameSize.split('x');
        await image.loadAtlas(framePaths, imageJson.hslices, imageJson.vslices, parseInt(frameSize[0]), parseInt(frameSize[1]));

        // If we are not bundling assets then we are writing an internal image and these defines would not be needed
        // This will be the case when writing materials for meshes
        if (bundleAssets){
            ImageAtlasDefines.writeHeaderFile(imageJson, assetDirectory, this._environment.outputDirectory);
        }

        return this._finalizeImage(image, options, assetDirectory, bundleAssets);
    }

    async _finalizeImage(image, imageJson, assetDir, bundleAssets) {
        if (imageJson.resize) {
            const dimensions = imageJson.resize.split("x");
            image.resize(parseInt(dimensions[0]), parseInt(dimensions[1]));
        }

        if (image.format === Image.Format.CI8 || image.format === Image.Format.CI4) {
            image.createColorIndexImage();

            if (imageJson.additionalPalettes) {
                for (const paletteFile of imageJson.additionalPalettes) {
                    const paletteFilePath = path.join(assetDir, paletteFile);
                    await image.colorIndexImage.addPaletteFromPath(paletteFilePath);
                }
            }
        }

        let assetIndex = -1;
        let assetBuffer = null;
        const imageWriter = new N64LibUltraImageWriter();

        if (bundleAssets) {
            const filePath = path.join(this._environment.outputDirectory, `${image.name}.image`);
            imageWriter.writeFile(image, imageJson.hslices, imageJson.vslices, filePath);
            assetIndex = this._environment.assetBundle.addImage(filePath, image.name);
        }
        else {
            assetBuffer = imageWriter.writeBuffer(image, imageJson.hslices, imageJson.vslices);
        }

        return {
            name: image.name,
            assetIndex: assetIndex,
            assetBuffer: assetBuffer,
            width: image.width,
            height: image.height,
            hslices: imageJson.hslices,
            vslices: imageJson.vslices,
        };
    }
};

module.exports = N64LibUltraImageProcessor;
