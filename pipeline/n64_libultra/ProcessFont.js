const Util = require("../Util");
const Font = require("./Font");
const FontUtils = require("../FontUtils");
const FontWriter = require("./FontWriter");
const ImageWriter = require("./ImageWriter");
const processImage = require("./ProcessImage");

const path = require("path")

function _initOptions(sourceFile, params) {
    const options = {
        sourceString: "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~<> ",
        name: path.basename(sourceFile, path.extname(sourceFile)) + params.size.toString(),
        imageFormat: "IA8"
    }

    Object.assign(options, params);

    return options;
}

async function processFontFile(manifestDirectory, outputDir, fontJson, archive) {
    if (!fontJson.size) {
        throw new Error("Font elements must have specify a size > 0.");
    }

    const sourceFile = path.join(manifestDirectory, fontJson.src);
    const options = _initOptions(sourceFile, fontJson);

    const safeFontName = Util.safeDefineName(options.name);
    const font = new Font(safeFontName);
    await font.loadGlyphs(sourceFile, options.sourceString, options.size);
    const image = await font.createFontImage(options.imageFormat);

    const hslices = image.width / font.tileWidth;
    const vslices = image.height / font.tileHeight;
    const imageBuffer = ImageWriter.writeBuffer(image, hslices, vslices);

    const fontFileName = safeFontName + ".font";
    const fontPath = path.join(outputDir, fontFileName);
    FontWriter.write(font, imageBuffer, fontPath);

    archive.addFont(fontPath, font.name);
}

async function processImageFont(manifestDirectory, outputDir, fontJson, archive) {
    if (!fontJson.sourceString) {
        throw new Error("image fonts must specify a sourceString");
    }

    /* this will create an image containing all the characters.  If each character has it's own file then they will be assembled
       into a single sprite using the combination facilities of the image processing code.
    */
    const imageInfo = await processImage(fontJson.image, null, manifestDirectory, null);

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
    const fontPath = path.join(outputDir, fontName);
    FontWriter.write(f, imageInfo.assetBuffer, fontPath);

    archive.addFont(fontPath, safeFontName);
}

async function processFont(manifestDirectory, outputDir, fontJson, archive) {
    if (fontJson.sourceFile) {
        const sourceFilePath = path.join(manifestDirectory, fontJson.sourceFile);
        fontJson.sourceString = FontUtils.sourceStringFromFile(sourceFilePath);
    }

    if (fontJson.src) {
        await processFontFile(manifestDirectory, outputDir, fontJson, archive);
    }
    else {
        await processImageFont(manifestDirectory, outputDir, fontJson, archive)
    }
}

module.exports = processFont;
