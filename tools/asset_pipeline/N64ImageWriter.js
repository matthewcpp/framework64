const fs = require("fs");

const itemsPerLine = 20;

function writeHeader(buffer, info, path, name) {
    const header = fs.openSync(path, "w")
    fs.writeSync(header, `#ifndef ${name.toUpperCase()}_TEXTURE_H\n`);
    fs.writeSync(header, `#define ${name.toUpperCase()}_TEXTURE_H\n\n`);

    fs.writeSync(header, `const int ${name}_info[] = {${info.width}, ${info.height}, ${info.hSlices}, ${info.vSlices}};\n\n`);

    fs.writeSync(header, `const unsigned char ${name}_image[] __attribute__ ((aligned (8))) = {`);

    for (let i =0; i < buffer.length; i++) {
        if (i % itemsPerLine === 0) {
            fs.writeSync(header, "\n    ");
        }

        fs.writeSync(header, `0x${buffer.readUInt8(i).toString(16)},`);
    }

    fs.writeSync(header, "\n\n};\n\n");
    fs.writeSync(header, "#endif\n")

    fs.closeSync(header);
}

module.exports = {
    writeHeader: writeHeader
}