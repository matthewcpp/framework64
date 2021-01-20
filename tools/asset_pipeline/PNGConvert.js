const N64Image = require("./N64Image")
const N64ImageWriter = require("./N64ImageWriter");

const fs = require("fs");
const path = require("path");


async function pngConvert(pngPath, outPath) {
    const image = new N64Image();
    await image.load(pngPath);
    const buffer = image.encode16bpp();

    const name = path.basename(pngPath,  ".png");

    N64ImageWriter.writeHeader(buffer, image, outPath, name);
}

async function main() {
    const { program } = require('commander');

    program.requiredOption("-f, --file <path>", "input file");
    program.requiredOption("-h --header-file <path>", "C header file output");

    program.parse(process.argv);

    if (!fs.existsSync(program.file)) {
        console.log(`Input file: ${program.file} does not exist`);
        process.exit(1);
    }

    await pngConvert(program.file, program.headerFile);
}

module.exports = pngConvert;

if (require.main === module) {
    main();
}

module.exports = pngConvert;