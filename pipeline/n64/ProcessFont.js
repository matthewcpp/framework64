const Util = require("../Util");
const Font = require("./Font");
const FontUtils = require("../FontUtils");
const DesktopFont = require("../desktop/Font");
const FontWriter = require("./FontWriter");
const ImageWriter = require("./ImageWriter");
const processImage = require("./ProcessImage");

const path = require("path")

function _initOptions(sourceFile, params) {
    const options = {
        sourceString: "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~ ",
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

    const font = new Font(options.name);
    await font.load(sourceFile);

    const fontData = await font.generateSpriteFont(options.sourceString, options.size, options.imageFormat);
    const hslices = fontData.image.width / fontData.tileWidth;
    const vslices = fontData.image.height / fontData.tileHeight;
    const imageBuffer = ImageWriter.writeBuffer(fontData.image, hslices, vslices);

    const fontFileName = Util.safeDefineName(font.name) + ".font";
    const fontPath = path.join(outputDir, fontFileName);
    FontWriter.write(fontData, imageBuffer, fontPath);

    await archive.add(fontPath, "font");
}

async function processImageFont(manifestDirectory, outputDir, fontJson, archive) {
    if (!fontJson.sourceString) {
        throw new Error("image fonts must specify a sourceString");
    }

    const imageInfo = await processImage(fontJson.image, null, manifestDirectory, null);

    const frameCount = imageInfo.hslices * imageInfo.vslices;
    if (frameCount !== fontJson.sourceString.length) {
        throw new Error(`Font image contains ${frameCount} frames but source string is of length: ${fontJson.sourceString.length}`);
    }

    const tileWidth = imageInfo.width / imageInfo.hslices;
    const tileHeight = imageInfo.height / imageInfo.vslices;

    const f = new DesktopFont();
    f.loadImageFontGlyphs(fontJson.name, fontJson.sourceString, tileWidth, tileHeight);

    const fontName = Util.safeDefineName(fontJson.name) + ".font";
    const fontPath = path.join(outputDir, fontName);
    FontWriter.write(f, imageInfo.assetBuffer, fontPath);

    await archive.add(fontPath, "font");
}

async function convertFont(manifestDirectory, outputDir, fontJson, archive) {
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

module.exports = {
    convertFont: convertFont
}
