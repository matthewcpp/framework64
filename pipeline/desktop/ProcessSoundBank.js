const AudioHeader = require("../AudioHeader")

const fs = require("fs");
const path = require("path");
const Util = require("../Util");

async function processSoundBank(soundBank, bundle, baseDirectory, outputDirectory, includeDirectory) {
    const sourceDir = path.join(baseDirectory, soundBank.dir);
    const destDir = path.join(outputDirectory, soundBank.name);

    fs.mkdirSync(destDir);

    const files = fs.readdirSync(sourceDir);

    for (let i = 0; i < files.length; i++) {
        const fileName = files[i];
        const srcFile = path.join(sourceDir, fileName);
        const destFile = path.join(destDir, `${i}.ogg`);
        fs.copyFileSync(srcFile, destFile);
    }

    bundle.addSoundBank(soundBank, files)
    AudioHeader.writeSoundBankHeader(sourceDir, includeDirectory);
}

module.exports = processSoundBank;