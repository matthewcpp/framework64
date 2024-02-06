const Util = require("./Util");

const fs = require("fs");
const path = require("path");

function writeMusicBankHeader(musicBankDir, name, outputDir) {
    const files = fs.readdirSync(musicBankDir);
    writeHeader("music_bank_" + name, files, outputDir);
}

function writeSoundBankHeader(files, name, outputDir) {
    writeHeader("sound_bank_" + name, files, outputDir);
}

function writeHeader(prefix, files, outputDir) {
    const includeFileName = Util.safeDefineName(prefix) + ".h";
    const includeFilePath = path.join(outputDir, includeFileName);

    const headerFile = fs.openSync(includeFilePath, "w");
    fs.writeSync(headerFile, "#pragma once\n\n");

    for (let i = 0; i < files.length; i++) {
        const fileName = files[i];
        const baseName = path.basename(fileName, path.extname(fileName));
        const define = Util.safeDefineName(`${prefix}_${baseName}`);

        fs.writeSync(headerFile, `#define ${define} ${i}\n`);
    }

    fs.closeSync(headerFile);
}

module.exports = {
    writeMusicBankHeader: writeMusicBankHeader,
    writeSoundBankHeader: writeSoundBankHeader
};
