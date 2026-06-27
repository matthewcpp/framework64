const Image = require("./Image");
const ImageAtlasDefines = require("../ImageAtlasDefines");
const ProcessImageUtils = require("../ProcessImageUtils");

const fs = require("fs-extra");
const path = require("path");
const DesktopImageWriter = require("./ImageWriter");
const Util = require("../Util");

class DesktopImageProcessor {
    _environment;

    constructor(environment) {
        this._environment = environment;
    }

    static imageIndexModes = new Set(["CI8", "CI4"]);

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
        if (imageJson.frames || imageJson.frameDir) {
            ImageAtlasDefines.writeHeaderFile(imageJson, assetDirectory, this._environment.outputDirectory);
            return this._buildAtlasFromFrames(imageJson, bundleAssets, assetDirectory);
        }
        else {
            return this._processSprite(imageJson, bundleAssets, assetDirectory)
        }
    }

    // todo: validate sprite params
    async _buildAtlasFromFrames(imageJson, bundleAsset, assetDirectory) {
        if (!Object.hasOwn(imageJson, "name")) {
            throw new Error("Sprite Atlas elements must specify a name");
        }

        const image = new Image(Util.safeDefineName(imageJson.name));
        
        const framePaths = ProcessImageUtils.getFramePathArray(imageJson, assetDirectory);
        const frameSize = imageJson.frameSize.split('x');
        await image.loadAtlas(framePaths, imageJson.hslices, imageJson.vslices, parseInt(frameSize[0]), parseInt(frameSize[1]));
        
        image.isIndexed = DesktopImageProcessor.imageIndexModes.has(imageJson.format);

        if (imageJson.resize) {
            this._resizeImage(image, imageJson);
        }

        if (bundleAsset) {
            const imageFileName = image.name + ".image";
            const destPath = path.join(this._environment.outputDirectory, imageFileName);
            await new DesktopImageWriter().writeFile(image, destPath);
            this._environment.assetBundle.addImage(imageFileName, image.name);
        }

        return image;
    }

    async _processSprite(imageJson, bundleAsset, assetDirectory) {
        const srcPath = path.join(assetDirectory, imageJson.src);
        const imageName = (!!imageJson.name) ? imageJson.name : path.basename(imageJson.src, path.extname(imageJson.src));
        const image = new Image(Util.safeDefineName(imageName));
        await image.load(srcPath);

        const indexMode = DesktopImageProcessor.imageIndexModes.has(imageJson.format);
        image.isIndexed = indexMode;

        if (indexMode && (!!imageJson.additionalPalettes)) {
            for (const palettePath of imageJson.additionalPalettes) {

                const paletteImageSrcPath = path.join(assetDirectory, palettePath);
                image.additionalPalettes.push(fs.readFileSync(paletteImageSrcPath));
            }
        }

        await this._processImage(image, imageJson, bundleAsset)

        return image;
    }

    _resizeImage(image, sprite) {
        const dimensions = sprite.resize.split("x");

        const width = parseInt(dimensions[0]);
        const height = parseInt(dimensions[1]);

        image.resize(width, height);
    }

    async _processImage(image, imageJson, bundleAsset) {
        const hslices = Object.hasOwn(imageJson, "hslices") ? imageJson.hslices : 1;
        const vslices = Object.hasOwn(imageJson, "vslices") ? imageJson.vslices : 1;
        image.setSliceCounts(hslices, vslices);
        
        if (Object.hasOwn(imageJson, "resize")) {
            this._resizeImage(image, imageJson);
        }

        if (bundleAsset) {
            const imageFileName = image.name + ".image";
            const destPath = path.join(this._environment.outputDirectory, imageFileName);
            fs.ensureDirSync(path.dirname(destPath));

            await new DesktopImageWriter().writeFile(image, destPath);
            this._environment.assetBundle.addImage(imageFileName, image.name);
        }
    }
};



module.exports = DesktopImageProcessor;