const N64ImageWriter = require("./N64ImageWriter");

const fs = require("fs");
const path = require("path");
const PNG = require("pngjs").PNG;

function encode16bpp(png) {
    const pixelCount = png.width * png.height;

    // rgba data (2 bytes per pixel)
    const bufferSize = pixelCount * 2;

    const buffer = Buffer.alloc(bufferSize);
    let offset = 0;

    for (let i = 0; i < pixelCount; i++) {
        let index = i * 4;

        // note the pixel data format is 5r 5g 5b 1a
        const r = png.data[index] >> 3;
        const g = png.data[index + 1] >> 3;
        const b = png.data[index + 2] >> 3;
        const a = png.data[index + 3] === 0 ? 0 : 1;

        const val = r << 11 | g << 6 | b << 1 | a;
        offset = buffer.writeUInt16BE(val, offset);
    }

    return buffer;
}

function pngConvert(pngPath, outPath) {
    const data = fs.readFileSync(pngPath);
    const png = PNG.sync.read(data);
    const buffer = encode16bpp(png);

    const spriteInfo = {
        width: png.width,
        height: png.height,
        hSlices: 1,
        vSlices: 1
    };

    const name = path.basename(pngPath,  ".png");

    N64ImageWriter.writeHeader(buffer, spriteInfo, outPath, name);
}


function main() {
    const { program } = require('commander');

    program.requiredOption("-f, --file <path>", "input file");
    program.requiredOption("-h --header-file <path>", "C header file output");

    program.parse(process.argv);

    if (!fs.existsSync(program.file)) {
        console.log(`Input file: ${program.file} does not exist`);
        process.exit(1);
    }

    pngConvert(program.file, program.headerFile);
}

module.exports = pngConvert;

if (require.main === module) {
    main();
}

module.exports = pngConvert;