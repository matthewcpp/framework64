const AudioHeader = require("../AudioHeader")

const fs = require("fs");
const path = require("path");
const Util = require("../Util");

async function processSoundBank(soundBank, bundle, baseDirectory, outputDirectory, includeDirectory) {
    const soundBankName = (!!soundBank.name) ? soundBank.name : path.basename(soundBank.dir);
    const sourceDir = path.join(baseDirectory, soundBank.dir);
    const destDirName = Util.safeDefineName(soundBankName);
    const destDir = path.join(outputDirectory, destDirName);

    fs.mkdirSync(destDir);

    const files = fs.readdirSync(sourceDir);

    for (let i = 0; i < files.length; i++) {
        const fileName = files[i];
        const srcFile = path.join(sourceDir, fileName);
        const destFile = path.join(destDir, `${i}.ogg`);
        fs.copyFileSync(srcFile, destFile);
    }

    const infoBuffer = Buffer.alloc(4);
    infoBuffer.writeUint32LE(files.length, 0);
    const infoFilePath = path.join(destDir, "info.soundbank");
    const infoFile = fs.openSync(infoFilePath, "w");
    fs.writeSync(infoFile, infoBuffer);
    fs.closeSync(infoFile)

    bundle.addSoundBank(soundBankName, destDirName)
    AudioHeader.writeSoundBankHeader(sourceDir, soundBankName, includeDirectory);
}

module.exports = processSoundBank;