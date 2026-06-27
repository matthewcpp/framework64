const Font = require("./Font");
const FontUtils = require("../FontUtils");
const DesktopFontWriter = require("./FontWriter");
const Util = require("../Util")

const path = require("path");

const ImageProcessor = require("./ImageProcessor");

class DesktopFontProcessor {
    _environment;
    _imageProcessor;

    static defaultSourceString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~<> ";

    constructor(environment, imageProcessor) {
        this._environment = environment;
        this._imageProcessor = imageProcessor;
    }

    async process(fontInfo) {
        if (fontInfo.sourceFile) {
            const sourceFilePath = path.join(this._environment.assetDirectory, fontInfo.sourceFile);
            fontInfo.sourceString = FontUtils.sourceStringFromFile(sourceFilePath);
        }

        if (fontInfo.src) {
            await this._processFontFile(fontInfo, this._environment.assetDirectory, this._environment.outputDirectory);
        } else {
            await this._processImageFont(fontInfo, this._environment.assetDirectory, this._environment.outputDirectory);
        }
    }

    async _processFontFile(fontInfo) {
        const fontName = this._determineFontName(fontInfo);
        const srcPath = path.join(this._environment.assetDirectory, fontInfo.src);
        const sourceString = fontInfo.sourceString ? fontInfo.sourceString : DesktopFontProcessor.defaultSourceString;

        const font = new Font(fontName);
        await font.loadGlyphs(srcPath, sourceString, fontInfo.size);
        await font.createGlImage();

        const fontFileName = fontName + ".font";
        const destPath = path.join(this._environment.outputDirectory, fontFileName);
        new DesktopFontWriter().writeFile(font, destPath);
        this._environment.assetBundle.addFont(fontFileName, font.name);
    }

    async _processImageFont(fontInfo) {
        if (!fontInfo.sourceString) {
            throw new Error("image fonts must specify an explicit sourceString");
        }

        if (!fontInfo.name) {
            throw new Error("image fonts must specify an explicit name");
        }

        const image = await this._imageProcessor.convertWithoutBundling(fontInfo.image, this._environment.assetDirectory);

        const frameCount = image.hslices * image.vslices;
        if (frameCount !== fontInfo.sourceString.length) {
            throw new Error(`Font image contains ${frameCount} frames but source string is of length: ${fontInfo.sourceString.length}`);
        }

        const font = new Font(Util.safeDefineName(fontInfo.name));
        const tileWidth = image.width / image.hslices;
        const tileHeight = image.height / image.vslices;
        font.image = image;
        font.loadImageFontGlyphs(fontInfo.name, fontInfo.sourceString, tileWidth, tileHeight);

        const fontFileName = font.name + ".font";
        const destPath = path.join(this._environment.outputDirectory, fontFileName);
        new DesktopFontWriter().writeFile(font, destPath);
        this._environment.assetBundle.addFont(fontFileName, font.name);
    }

    _determineFontName(fontInfo) {
        if (fontInfo.name) {
            return Util.safeDefineName(fontInfo.name);
        }

        const basename = path.basename(fontInfo.src, path.extname(fontInfo.src));
        return Util.safeDefineName(basename + fontInfo.size.toString());
    }
};

module.exports = DesktopFontProcessor;
