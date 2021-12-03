const Util = require("../Util");
const N64Image = require("./N64Image");
const N64Font = require("./N64Font");
const Font = require("../Font");
const N64FontWriter = require("./N64FontWriter");
const N64ImageWriter = require("./N64ImageWriter");
const processImage = require("./ProcessImage");



const path = require("path")

function _initOptions(sourceFile, params) {
    const options = {
        sourceString: "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+[];:',./\\\"`~ ",
        name: path.basename(sourceFile, path.extname(sourceFile)) + params.size.toString(),
        imageFormat: "rgba16"
    }

    Object.assign(options, params);

    return options;
}

async function processFontFile(manifestDirectory, outputDir, params, archive) {
    if (!params.size) {
        throw new Error("Font elements must have a size specified.");
    }

    const sourceFile = path.join(manifestDirectory, params.src);

    const options = _initOptions(sourceFile, params);

    const font = new N64Font(options.name);
    await font.load(sourceFile);

    const data = await font.generateSpriteFont(options.sourceString, options.size, options.imageFormat);
    const fontPath = path.join(outputDir, `${font.name}.font`);
    const imagePath = path.join(outputDir, `${font.name}_image.image`);

    const hslices = data.image.width / data.tileWidth;
    const vslices = data.image.height / data.tileHeight;

    N64ImageWriter.writeBinary(data.image, hslices, vslices, imagePath);
    const imageInfo = await archive.add(imagePath, "image");

    N64FontWriter.write(data, imageInfo.index, fontPath);
    await archive.add(fontPath, "font");
}

async function processImageFont(manifestDirectory, outputDir, font, archive) {
    if (!font.sourceString) {
        throw new Error("image fonts must specify a sourceString");
    }

    const imageInfo = await processImage(manifestDirectory, outputDir, font.image, archive);

    const frameCount = imageInfo.hslices * imageInfo.vslices;
    if (frameCount !== font.sourceString.length) {
        throw new Error(`Font image contains ${frameCount} frames but source string is of length: ${font.sourceString.length}`);
    }

    const tileWidth = imageInfo.width / imageInfo.hslices;
    const tileHeight = imageInfo.height / imageInfo.vslices;

    const f = new Font();
    f.loadImageFontGlyphs(font.name, font.sourceString, tileWidth, tileHeight);

    const fontPath = path.join(outputDir, Util.safeDefineName(font.name) + ".font");
    N64FontWriter.write(f, imageInfo.assetIndex, fontPath);

    await archive.add(fontPath, "font");
}

async function convertFont(manifestDirectory, outputDir, font, archive) {
    if (font.sourceFile) {
        const sourceFilePath = path.join(manifestDirectory, font.sourceFile);
        font.sourceString = Font.sourceStringFromFile(sourceFilePath);
    }

    if (font.src) {
        await processFontFile(manifestDirectory, outputDir, font, archive);
    }
    else {
        await processImageFont(manifestDirectory, outputDir, font, archive)
    }
}

module.exports = {
    convertFont: convertFont
}

const paramKeys = ["size", "sourceString"];

function _buildParams(program) {
    const params = {};

    for (const key of paramKeys) {
        if (program.hasOwnProperty(key))
            params[key] = program[key];
    }

    return params;
}

async function main() {
    const { program } = require('commander');

    program.version("0.0.1");
    program.requiredOption("-f, --file <path>", "input file");
    program.requiredOption("--size <value>", "font size");
    program.requiredOption("-o, --out-dir <dir>", "output directory");
    program.option("--source-string <value>", "String to collect glyphs from")

    program.parse(process.argv);

    const options = _initOptions(program.file, _buildParams(program));

    const font = new N64Font(options.name);
    await font.load(program.file);
    await font.outputToDirectory(program.outDir, options.sourceString, options.size, N64Image.Format.RGBA32);
}

if (require.main === module) {
    main();
}