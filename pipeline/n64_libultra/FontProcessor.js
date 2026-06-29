const Font = require("./Font");
const FontUtils = require("../FontUtils");
const Image = require("./Image");
const N64LibUltraFontWriter = require("./FontWriter");
const N64LibUltraImageProcessor = require("./ImageProcessor");
const N64LibUltraImageWriter = require("./ImageWriter");
const Util = require("../Util");

const processImage = require("./ImageProcessor");

const path = require("path")

class N64LibUltraFontProcessor {
    _environment;

    constructor(environment) {
        this._environment = environment;
    }

    async process(fontJson) {
        if (fontJson.sourceFile) {
            const sourceFilePath = path.join(this._environment.assetDirectory, fontJson.sourceFile);
            fontJson.sourceString = FontUtils.sourceStringFromFile(sourceFilePath);
        }

        if (fontJson.src) {
            await this._processFontFile(fontJson);
        } else {
            await this._processImageFont(fontJson);
        }
    }

    async _processFontFile(fontJson) {
        if (!Object.hasOwn(fontJson, "size") || fontJson.size <= 0) {
            throw new Error("Font elements must have specify a size > 0.");
        }

        const sourceFile = path.join(this._environment.assetDirectory, fontJson.src);
        const options = this._initOptions(sourceFile, fontJson);

        const safeFontName = Util.safeDefineName(options.name);
        const font = new Font(safeFontName);
        await font.loadGlyphs(sourceFile, options.sourceString, options.size);
        const image = await font.createFontImage(Image.Format[options.imageFormat.toUpperCase()]);

        const hslices = image.width / font.tileWidth;
        const vslices = image.height / font.tileHeight;
        const imageBuffer = new N64LibUltraImageWriter().writeBuffer(image, hslices, vslices);

        const fontFileName = safeFontName + ".font";
        const fontPath = path.join(this._environment.outputDirectory, fontFileName);
        new N64LibUltraFontWriter().writeBinary(font, imageBuffer, fontPath);

        this._environment.assetBundle.addFont(fontPath, font.name);
    }

    async _processImageFont(fontJson) {
        if (!fontJson.sourceString) {
            throw new Error("image fonts must specify a sourceString");
        }

        const imageProcessor = new N64LibUltraImageProcessor(this._environment);

        // this will create an image containing all the characters.  If each character has it's own file then they will be assembled
        // into a single sprite using the combination facilities of the image processing code.
        const imageInfo = await imageProcessor.convertWithoutBundling(fontJson.image, this._environment.assetDirectory);

        const frameCount = imageInfo.hslices * imageInfo.vslices;
        if (frameCount !== fontJson.sourceString.length) {
            throw new Error(`Font image contains ${frameCount} frames but source string is of length: ${fontJson.sourceString.length}`);
        }

        const tileWidth = imageInfo.width / imageInfo.hslices;
        const tileHeight = imageInfo.height / imageInfo.vslices;

        const safeFontName = Util.safeDefineName(fontJson.name);
        const f = new Font(safeFontName);
        f.loadImageFontGlyphs(fontJson.name, fontJson.sourceString, tileWidth, tileHeight);

        const fontName = safeFontName + ".font";
        const fontPath = path.join(this._environment.outputDirectory, fontName);
        new N64LibUltraFontWriter().writeBinary(f, imageInfo.assetBuffer, fontPath);

        this._environment.assetBundle.addFont(fontPath, safeFontName);
    }

    _initOptions(sourceFile, params) {
        const options = {
            sourceString: "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~<> ",
            name: path.basename(sourceFile, path.extname(sourceFile)) + params.size.toString(),
            imageFormat: "IA8"
        }

        Object.assign(options, params);

        return options;
    }
};

module.exports = N64LibUltraFontProcessor;
