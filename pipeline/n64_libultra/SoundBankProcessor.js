const AudioHeader = require("../AudioHeader");
const SoundBank = require("./SoundBank");

const tmp = require("tmp");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

class N64LibUltraSoundBankProcessor {
    _environment;

    constructor(environment) {
        this._environment = environment;
    }
    async process(soundBank) {
        const sourceDir = path.join(this._environment.assetDirectory, soundBank.dir);
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
            N64LibUltraSoundBankProcessor._checkOutputPaths([ctrlFilePath, tblFilePath, jsonFilePath]);

            const output = JSON.parse(fs.readFileSync(jsonFilePath, {encoding: "utf8"}));

            const soundBankPath = path.join(this._environment.outputDirectory, name + ".soundbank");
            SoundBank.writeFile(soundBankPath, ctrlFilePath, tblFilePath, output.length);
            this._environment.assetBundle.addSoundBank(soundBankPath, name);
            const files = fs.readdirSync(sourceDir);
            AudioHeader.writeSoundBankHeader(output, name, this._environment.includeDirectory);
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

    static _checkOutputPaths(paths) {
        for (const path of paths) {
            if (!fs.existsSync(path))
                throw new Error(`Docker container did not signal conversion failure but ${path} does not exist.`);
        }
    }
};

module.exports = N64LibUltraSoundBankProcessor
