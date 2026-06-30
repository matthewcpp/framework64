const AudioHeader = require("../AudioHeader");

const fs = require("fs");
const path = require("path");
const Util = require("../Util");

class DesktopSoundBankProcessor {
    static supportedSoundFileExtensions = new Set([".ogg", ".wav"]);

    _environment;

    constructor(environment) {
        this._environment = environment;
    }

    async process(soundBank) {
        const soundBankName = (!!soundBank.name) ? soundBank.name : path.basename(soundBank.dir);
        const sourceDir = path.join(this._environment.assetDirectory, soundBank.dir);
        const destDirName = Util.safeDefineName(soundBankName);
        const destDir = path.join(this._environment.outputDirectory, destDirName);

        fs.mkdirSync(destDir);

        const files = fs.readdirSync(sourceDir);
        const soundBankFiles = [];
        for (let i = 0; i < files.length; i++) {
            const fileName = files[i];
            const ext = path.extname(fileName);

            if (!DesktopSoundBankProcessor.supportedSoundFileExtensions.has(ext)) {
                console.log(`Soundbank ${soundBankName} Warning: skipping file with unsupported extension: ${fileName}.`);
                continue;
            }

            const srcFile = path.join(sourceDir, fileName);
            const destFile = path.join(destDir, soundBankFiles.length.toString() + ext);
            fs.copyFileSync(srcFile, destFile);
            soundBankFiles.push(srcFile);
        }

        const infoBuffer = Buffer.alloc(4);
        infoBuffer.writeUint32LE(soundBankFiles.length, 0);
        const infoFilePath = path.join(destDir, "info.soundbank");
        const infoFile = fs.openSync(infoFilePath, "w");
        fs.writeSync(infoFile, infoBuffer);
        fs.closeSync(infoFile)

        this._environment.assetBundle.addSoundBank(destDirName, soundBankName);
        AudioHeader.writeSoundBankHeader(soundBankFiles, soundBankName, this._environment.includeDirectory);
    }
};

module.exports = DesktopSoundBankProcessor;
