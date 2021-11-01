const tmp = require("tmp");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

function writeSoundBankFile(soundBankPath, ctlFilePath, tblFilePath, fileCount) {
    const ctrlFileStats = fs.statSync(ctlFilePath);

    const soundBankBuffer = Buffer.alloc(8);
    soundBankBuffer.writeUInt32BE(fileCount, 0);
    soundBankBuffer.writeUInt32BE(ctrlFileStats.size, 4);

    const soundBankFile = fs.openSync(soundBankPath, "w");
    fs.writeSync(soundBankFile, soundBankBuffer);
    fs.writeSync(soundBankFile, fs.readFileSync(ctlFilePath));
    fs.writeSync(soundBankFile, fs.readFileSync(tblFilePath));
    fs.closeSync(soundBankFile);
}

function checkOutputPaths(paths) {
    for (const path of paths) {
        if (!fs.existsSync(path))
            throw new Error(`Docker container did not signal conversion failure but ${path} does not exist.`);
    }
}

async function convertSoundBank(sourceDir, name, outputDir, archive) {
    if (!fs.existsSync(sourceDir)) {
        throw new Error(`Source directory: ${sourceDir} does not exist`);
    }

    const tempDir = tmp.dirSync({unsafeCleanup: true});

    const conversionArgs = [
        "run", "--rm",
        "-v", `${sourceDir}:/src`,
        "-v", `${tempDir.name}:/dest`,
        "matthewcpp/framework64-audio",
        "create_sound_bank", name
    ];

    try {
        console.log(`Creating sound bank: ${name} from ${sourceDir}`);
        await execFile("docker", conversionArgs);
        console.log(`Successfully created: ${name}`);

        const ctrlFilePath = path.join(tempDir.name, name + ".ctl");
        const tblFilePath = path.join(tempDir.name, name + ".tbl");
        const jsonFilePath = path.join(tempDir.name, name + ".json");
        checkOutputPaths([ctrlFilePath, tblFilePath, jsonFilePath]);

        const output = JSON.parse(fs.readFileSync(jsonFilePath, {encoding: "utf8"}));

        const soundBankPath = path.join(outputDir, name + ".soundbank");
        writeSoundBankFile(soundBankPath, ctrlFilePath, tblFilePath, output.length);
        await archive.add(soundBankPath, "soundbank");
    }
    catch (e) {
        console.log(e);
        console.log("docker", conversionArgs.join(' '));
        throw (e);
    }
    finally {
        tempDir.removeCallback();
    }
}

async function convertMusicBank(sourceDir, name, outputDir, archive) {
    if (!fs.existsSync(sourceDir)) {
        throw new Error(`Source directory: ${sourceDir} does not exist`);
    }

    const tempDir = tmp.dirSync({unsafeCleanup: true});

    const conversionArgs = [
        "run", "--rm",
        "-v", `${sourceDir}:/src`,
        "-v", `${tempDir.name}:/dest`,
        "matthewcpp/framework64-audio",
        "create_sequence_bank", name
    ];

    try {
        console.log(`Creating sequence bank: ${name} from ${sourceDir}`);
        await execFile("docker", conversionArgs);
        console.log(`Successfully created: ${name}`);

        const ctrlFilePath = path.join(tempDir.name, name + ".ctl");
        const tblFilePath = path.join(tempDir.name, name + ".tbl");
        const sbkFilePath = path.join(tempDir.name, name + ".sbk");
        const jsonFilePath = path.join(tempDir.name, name + ".json");
        checkOutputPaths([ctrlFilePath, tblFilePath, sbkFilePath, jsonFilePath]);

        const output = JSON.parse(fs.readFileSync(jsonFilePath, {encoding: "utf8"}));

        const instrumentBankPath = path.join(outputDir, `${name}.instrumentbank`);
        writeSoundBankFile(instrumentBankPath, ctrlFilePath, tblFilePath, 0);
        const entry = await archive.add(instrumentBankPath, "instrumentbank");

        const sbkFileStats = fs.statSync(sbkFilePath);

        const musicBankHeader = Buffer.alloc(12);
        musicBankHeader.writeUInt32BE(output.length, 0);
        musicBankHeader.writeUInt32BE(entry.index, 4);
        musicBankHeader.writeUInt32BE(sbkFileStats.size, 8);

        const musicBankPath = path.join(outputDir, `${name}.musicbank`);
        const musicBankFile = fs.openSync(musicBankPath, "w");
        fs.writeSync(musicBankFile, musicBankHeader);
        fs.writeSync(musicBankFile, fs.readFileSync(sbkFilePath));
        fs.closeSync(musicBankFile);

        await archive.add(musicBankPath, "musicbank");
    }
    catch (e) {
        console.log(e);
        console.log("docker", conversionArgs.join(' '));
        throw (e);
    }
    finally {
        tempDir.removeCallback();
    }
}

module.exports = {
    convertSoundBank: convertSoundBank,
    convertMusicBank: convertMusicBank
};
