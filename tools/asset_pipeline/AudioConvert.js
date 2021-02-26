const N64SoundBank = require("./N64SoundBank");
const N64AudioWriter = require("./N64AudioWriter");

const fs = require("fs");
const path = require("path");

async function convertSoundBank(files, name, outputDir, archive) {
    const soundBank = new N64SoundBank(name);
    soundBank.load(files);
    await N64AudioWriter.writeSoundBank(soundBank);

    const ctrlFileStats = fs.statSync(soundBank.ctlFilePath);

    const soundBankBuffer = Buffer.alloc(8);
    soundBankBuffer.writeUInt32BE(soundBank.fileCount, 0);
    soundBankBuffer.writeUInt32BE(ctrlFileStats.size, 4);

    const soundBankPath = path.join(outputDir, `${soundBank.name}.soundbank`);

    const soundBankFile = fs.openSync(soundBankPath, "w");
    fs.writeSync(soundBankFile, soundBankBuffer);
    fs.writeSync(soundBankFile, fs.readFileSync(soundBank.ctlFilePath));
    fs.writeSync(soundBankFile, fs.readFileSync(soundBank.tblFilePath));
    fs.closeSync(soundBankFile);

    archive.add(soundBankPath, "soundbank");

    soundBank.cleanup();
}

module.exports = {
    convertSoundBank: convertSoundBank
};
