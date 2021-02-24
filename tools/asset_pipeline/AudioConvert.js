const N64SoundBank = require("./N64SoundBank");
const N64AudioWriter = require("./N64AudioWriter");

const fs = require("fs");
const path = require("path");

async function convertSoundBank(files, name, outputDir, archive) {
    const soundBank = new N64SoundBank(name);
    soundBank.load(files);
    await N64AudioWriter.writeSoundBank(soundBank);

    fs.copyFileSync(soundBank.tblFilePath, path.join(outputDir, soundBank.tblFileName));
    fs.copyFileSync(soundBank.ctlFilePath, path.join(outputDir, soundBank.ctlFileName));

    soundBank.cleanup();
}

module.exports = {
    convertSoundBank: convertSoundBank
};
