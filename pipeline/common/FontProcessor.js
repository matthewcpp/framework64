const FontUtils = require("../FontUtils");
const Util = require("../Util")

const path = require("path");

class FontProcessor {
    _environment;
    _imageProcessor;
    _fontWriter;

    static defaultSourceString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~<> ";

    constructor(environment, imageProcessor, fontWriter) {
        this._environment = environment;
        this._imageProcessor = imageProcessor;
        this._fontWriter = fontWriter;
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
        if (!Object.hasOwn(fontInfo, "size") || fontInfo.size <= 0) {
            throw new Error("Font elements must have specify a size > 0.");
        }

        const fontName = this._determineFontName(fontInfo);
        const srcPath = path.join(this._environment.assetDirectory, fontInfo.src);
        const sourceString = fontInfo.sourceString ? fontInfo.sourceString : FontProcessor.defaultSourceString;

        const font = this._createFont(fontName);
        await font.loadGlyphs(srcPath, sourceString, fontInfo.size);

        const fontFileName = fontName + ".font";
        const destPath = path.join(this._environment.outputDirectory, fontFileName);
        await this._fontWriter.writeFile(font, destPath);
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

        const font = this._createFont(Util.safeDefineName(fontInfo.name));
        const tileWidth = image.width / image.hslices;
        const tileHeight = image.height / image.vslices;
        font.image = image;
        font.loadImageFontGlyphs(fontInfo.name, fontInfo.sourceString, tileWidth, tileHeight);

        const fontFileName = font.name + ".font";
        const destPath = path.join(this._environment.outputDirectory, fontFileName);
        await this._fontWriter.writeFile(font, destPath);
        this._environment.assetBundle.addFont(fontFileName, font.name);
    }

    _determineFontName(fontInfo) {
        if (fontInfo.name) {
            return Util.safeDefineName(fontInfo.name);
        }

        const basename = path.basename(fontInfo.src, path.extname(fontInfo.src));
        return Util.safeDefineName(basename + fontInfo.size.toString());
    }

    /** Note each platform needs to implement this method so that the processor can create the appropriate platform dependant font object */
    _createFont(name) {
        throw new Error("Derived classes must implement the _createFont method");
    }
};

module.exports = FontProcessor;
