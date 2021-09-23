const Font = require("../Font");

const path = require("path");

const defaultSourceString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~ ";

async function processFont(fontInfo, bundle, baseDirectory, outputDirectory) {
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

module.exports = processFont;