const Font = require("../Font");

const processImage = require("./ProcessImage");

const path = require("path");

const defaultSourceString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~ ";

async function processFontFile(fontInfo, bundle, baseDirectory, outputDirectory) {
    const fontName = fontInfo.name + ".png";
    const srcPath = path.join(baseDirectory, fontInfo.src);
    const destPath = path.join(outputDirectory, fontName);
    const sourceString = fontInfo.sourceString ? fontInfo.sourceString : defaultSourceString;

    const font = new Font();
    await font.loadGlyphs(fontInfo.name, srcPath, sourceString, fontInfo.size);
    await font.createGlImage();

    await font.image.writeToFile(destPath);
    bundle.addFont(font, fontName);
}

async function processImageFont(fontInfo, bundle, baseDirectory, outputDirectory) {
    if (!fontInfo.sourceString) {
        throw new Error("image fonts must specify a sourceString");
    }

    const imageInfo = await processImage(fontInfo.image, bundle, baseDirectory, outputDirectory);

    const frameCount = imageInfo.hslices * imageInfo.vslices;
    if (frameCount !== fontInfo.sourceString.length) {
        throw new Error(`Font image contains ${frameCount} frames but source string is of length: ${fontInfo.sourceString.length}`);
    }

    const tileWidth = imageInfo.width / imageInfo.hslices;
    const tileHeight = imageInfo.height / imageInfo.vslices;

    const f = new Font();
    f.loadImageFontGlyphs(fontInfo.name, fontInfo.sourceString, tileWidth, tileHeight);

    const fontName = path.basename(imageInfo.path);
    bundle.addFont(f, fontName);
}

async function processFont(fontInfo, bundle, baseDirectory, outputDirectory) {
    if (fontInfo.sourceFile) {
        const sourceFilePath = path.join(baseDirectory, fontInfo.sourceFile);
        fontInfo.sourceString = Font.sourceStringFromFile(sourceFilePath);
    }

    if (fontInfo.src) {
        await processFontFile(fontInfo, bundle, baseDirectory, outputDirectory);
    }
    else {
        await processImageFont(fontInfo, bundle, baseDirectory, outputDirectory);
    }
}

module.exports = processFont;