const Font = require("./Font");
const FontWriter = require("./FontWriter");
const Util = require("../Util")
const FontUtils = require("../FontUtils");
const processImage = require("./ProcessImage");

const path = require("path");

const defaultSourceString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~<> ";

async function processFontFile(fontInfo, bundle, baseDirectory, outputDirectory) {
    const fontName = determineFontName(fontInfo);

    const srcPath = path.join(baseDirectory, fontInfo.src);

    const sourceString = fontInfo.sourceString ? fontInfo.sourceString : defaultSourceString;

    const font = new Font(fontName);
    await font.loadGlyphs(srcPath, sourceString, fontInfo.size);
    await font.createGlImage();

    const fontFileName = fontName + ".font";
    const destPath = path.join(outputDirectory, fontFileName);
    FontWriter.writeFile(font, destPath);
    bundle.addFont(font.name, fontFileName);
}

async function processImageFont(fontInfo, bundle, baseDirectory, outputDirectory) {
    if (!fontInfo.sourceString) {
        throw new Error("image fonts must specify an explicit sourceString");
    }

    if (!fontInfo.name) {
        throw new Error("image fonts must specify an explicit name");
    }

    const image = await processImage(fontInfo.image, null, baseDirectory, outputDirectory);

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
    const destPath = path.join(outputDirectory, fontFileName);
    FontWriter.writeFile(font, destPath)
    bundle.addFont(font.name, fontFileName);
}

async function processFont(fontInfo, bundle, baseDirectory, outputDirectory) {
    if (fontInfo.sourceFile) {
        const sourceFilePath = path.join(baseDirectory, fontInfo.sourceFile);
        fontInfo.sourceString = FontUtils.sourceStringFromFile(sourceFilePath);
    }

    if (fontInfo.src) {
        await processFontFile(fontInfo, bundle, baseDirectory, outputDirectory);
    }
    else {
        await processImageFont(fontInfo, bundle, baseDirectory, outputDirectory);
    }
}

function determineFontName(fontInfo) {
    if (fontInfo.name)
        return Util.safeDefineName(fontInfo.name);

    const basename = path.basename(fontInfo.src, path.extname(fontInfo.src));
    return Util.safeDefineName(basename + fontInfo.size.toString());
}

module.exports = processFont;