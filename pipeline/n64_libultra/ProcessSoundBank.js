const AudioHeader = require("../AudioHeader");
const SoundBank = require("./SoundBank");

const tmp = require("tmp");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

function checkOutputPaths(paths) {
    for (const path of paths) {
        if (!fs.existsSync(path))
            throw new Error(`Docker container did not signal conversion failure but ${path} does not exist.`);
    }
}

async function processSoundBank(environment, soundBank) {
    const sourceDir = path.join(environment.assetDirectory, soundBank.dir);
    const name = soundBank.name;

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

        const soundBankPath = path.join(environment.outputDirectory, name + ".soundbank");
        SoundBank.writeFile(soundBankPath, ctrlFilePath, tblFilePath, output.length);
        environment.assetBundle.addSoundBank(soundBankPath, name);
        const files = fs.readdirSync(sourceDir);
        AudioHeader.writeSoundBankHeader(output, name, environment.includeDirectory);
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

module.exports = processSoundBank