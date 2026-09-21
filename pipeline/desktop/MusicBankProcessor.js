const AudioHeader = require("../AudioHeader");
const Util = require("../Util");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

class DesktopMusicBankProcessor {
    _environment;

    static midiFileExtensions = new Set([".mid", ".midi"]);
    static midiAlternateExtension = ".desktop_ogg";

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

            const destFile = `${musicBankFiles.length}.ogg`;

            if (DesktopMusicBankProcessor.midiFileExtensions.has(ext)) {
                // check if there is a valid alternative audio file we can use in lieu of converting the midi
                const sourceFile = path.join(sourceDir, file);
                const alternativeAudioFileName = path.basename(sourceFile, path.extname(sourceFile)) + DesktopMusicBankProcessor.midiAlternateExtension;
                const alternativeAudioPath = path.join(path.dirname(sourceFile), alternativeAudioFileName);

                // if a desktop alternative is present, no need to convert, simply copy it over
                if (fs.existsSync(alternativeAudioPath)) {
                    console.log(`Musicbank ${musicBankName}: ${file}: using desktop alternative file: ${path.basename(alternativeAudioPath)}`);
                    const destFilePath = path.join(destDir, destFile);
                    fs.copyFileSync(alternativeAudioPath, destFilePath);
                } else {
                    await this._convertMidiToOgg(sourceDir, file, destDir, destFile);
                }
            }
            // if the music file is already in ogg format then no conversion is needed, just a copy to the destination
            else if (ext === ".ogg") {
                const sourceFilePath = path.join(sourceDir, file);
                const destFilePath = path.join(destDir, destFile);
                fs.copyFileSync(sourceFilePath, destFilePath);
            }
            else {
                if (ext !== DesktopMusicBankProcessor.midiAlternateExtension) {
                    console.log(`Musicbank ${musicBankName} Warning: skipping file with unsupported extension: ${file}.`);
                }

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