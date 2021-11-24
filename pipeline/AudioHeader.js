const Util = require("./Util");

const fs = require("fs");
const path = require("path");

function writeMusicBankHeader(musicBankDir, outputDir) {
    writeHeader("music_bank", path.basename(musicBankDir), musicBankDir, outputDir);
}

function writeSoundBankHeader(soundBankDir, outputDir) {
    writeHeader("sound_bank", path.basename(soundBankDir), soundBankDir, outputDir);
}

function writeHeader(typePrefix, namePrefix, sourceDir, outputDir) {
    const includeFileName = Util.safeDefineName(`${typePrefix}_${namePrefix}.h`);
    const includeFilePath = path.join(outputDir, "include", includeFileName);

    const headerFile = fs.openSync(includeFilePath, "w");
    fs.writeSync(headerFile, "#pragma once\n\n");

    const files = fs.readdirSync(sourceDir);

    for (let i = 0; i < files.length; i++) {
        const fileName = files[i];
        const baseName = path.basename(fileName, path.extname(fileName));
        const define = Util.safeDefineName(`${typePrefix}_${namePrefix}_${baseName}`);

        fs.writeSync(headerFile, `#define ${define} ${i}\n`);
    }

    fs.closeSync(headerFile);
}

module.exports = {
    writeMusicBankHeader: writeMusicBankHeader,
    writeSoundBankHeader: writeSoundBankHeader
};
