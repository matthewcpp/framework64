const N64Image = require("./N64Image");

const fs = require("fs");

const itemsPerLine = 32;

function writeDataArray(file, name, data, width, height) {
    const buffer = N64Image.encode16bpp(data, width, height);

    fs.writeSync(file, `unsigned char ${name}[] __attribute__ ((aligned (8))) = {`);

    for (let i =0; i < buffer.length; i++) {
        if (i % itemsPerLine === 0) {
            fs.writeSync(file, "\n    ");
        }

        fs.writeSync(file, `0x${buffer.readUInt8(i).toString(16)},`);
    }

    fs.writeSync(file, "\n\n};\n\n");
}

module.exports = {
    writeDataArray: writeDataArray
}