const AudioHeader = require("../AudioHeader");
const Util = require("../Util");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

class DesktopMusicBankProcessor {
    _environment;

    static midiFileExtensions = new Set([".mid", ".midi"]);

    constructor(environment) {
        this._environment = environment;
    }

    async process(musicBank) {
        const musicBankName = (!!musicBank.name) ? musicBank.name : path.basename(musicBank.dir);
        const sourceDir = path.join(this._environment.assetDirectory, musicBank.dir);
        const destDirName = Util.safeDefineName(musicBankName);
        const destDir = path.join(this._environment.outputDirectory, destDirName);

        fs.mkdirSync(destDir);

        const files = fs.readdirSync(sourceDir);
        const musicBankFiles = [];
        for (let i = 0; i < files.length; i++) {
            const file = files[i];
            const ext = path.extname(file);

            const destFile = `${musicBankFiles.length}.ogg`;  path.join(destDir, );

            if (DesktopMusicBankProcessor.midiFileExtensions.has(ext)) {
                await this._convertMidiToOgg(sourceDir, file, destDir, destFile);
            }
            else if (ext === ".ogg") {
                const sourceFilePath = path.join(sourceDir, file);
                const destFilePath = path.join(destDir, destFile);

                fs.copyFileSync(sourceFilePath, destFilePath);
            }
            else {
                console.log(`Musicbank ${musicBankName} Warning: skipping file with unsupported extension: ${file}.`);
                continue;
            }
            
            musicBankFiles.push(path.join(sourceDir, file));
        }

        const infoBuffer = Buffer.alloc(4);
        infoBuffer.writeUint32LE(musicBankFiles.length, 0);
        const infoFilePath = path.join(destDir, "info.musicbank");
        const infoFile = fs.openSync(infoFilePath, "w");
        fs.writeSync(infoFile, infoBuffer);
        fs.closeSync(infoFile)

        this._environment.assetBundle.addMusicBank(destDirName, musicBankName);
        AudioHeader.writeMusicBankHeader(musicBankFiles, musicBankName, this._environment.includeDirectory);
    }

    async _convertMidiToOgg(sourceDir, sourceFile, destDir, destFile) {
        const conversionArgs = [
            "run", "--rm",
            "--mount", `type=bind,source=${sourceDir},target=/src`,
            "--mount", `type=bind,source=${destDir},target=/dest`,
            "matthewcpp/midi2snd",
            sourceFile, destFile
        ];

        await execFile("docker", conversionArgs);
    }
};

module.exports = DesktopMusicBankProcessor;