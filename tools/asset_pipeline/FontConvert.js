const N64Font = require("./N64Font")
const N64FontWriter = require("./N64FontWriter");

const path = require("path")

function _initOptions(sourceFile, params) {
    const options = {
        sourceString: "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+[];:',./\\\"`~ ",
        name: path.basename(sourceFile, path.extname(sourceFile)) + params.size.toString()
    }

    Object.assign(options, params);

    return options;
}

async function convertFont(sourceFile, outputDir, params, archive) {
    if (!params.size) {
        throw new Error("Font elements must have a size specified.");
    }

    const options = _initOptions(sourceFile, params);

    const font = new N64Font(options.name);
    await font.load(sourceFile);

    const data = await font.generateSpriteFont(options.sourceString, options.size);
    const fontPath = path.join(outputDir, `${font.name}.font`);
    archive.add(fontPath, "font");
    N64FontWriter.write(data, fontPath);
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
    await font.outputToDirectory(program.outDir, options.sourceString, options.size);
}

if (require.main === module) {
    main();
}