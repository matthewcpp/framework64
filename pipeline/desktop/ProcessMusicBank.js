const AudioHeader = require("../AudioHeader");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const midiFileExtensions = new Set([".mid", ".midi"]);

async function convertMidiToOgg(sourceDir, sourceFile, destDir, destFile) {
    const conversionArgs = [
        "run", "--rm",
        "--mount", `type=bind,source=${sourceDir},target=/src`,
        "--mount", `type=bind,source=${destDir},target=/dest`,
        "matthewcpp/midi2snd",
        sourceFile, destFile
    ];

    await execFile("docker", conversionArgs);
}

async function processMusicBank(musicBank, bundle, baseDirectory, outputDirectory) {
    const sourceDir = path.join(baseDirectory, musicBank.dir);
    const destDir = path.join(outputDirectory, musicBank.name);

    fs.mkdirSync(destDir);

    const files = fs.readdirSync(sourceDir);

    for (let i = 0; i < files.length; i++) {
        const file = files[i];
        const ext = path.extname(file);

        const destFile = `${i}.ogg`;  path.join(destDir, );

        if (midiFileExtensions.has(ext)) {
            await convertMidiToOgg(sourceDir, file, destDir, destFile);
        }
        else if (ext === ".ogg") {
            const sourceFilePath = path.join(sourceDir, file);
            const destFilePath = path.join(destDir, destFile);

            fs.copyFileSync(sourceFilePath, destFilePath);
        }
    }

    bundle.addMusicBank(musicBank, files);
    AudioHeader.writeMusicBankHeader(sourceDir, outputDirectory);
}

module.exports = processMusicBank;