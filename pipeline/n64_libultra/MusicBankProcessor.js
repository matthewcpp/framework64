const AudioHeader = require("../AudioHeader");
const SoundBank = require("./SoundBank");

const tmp = require("tmp");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

class N64LibUltraMusicBankProcessor {
    _environment;

    constructor(environment) {
        this._environment = environment;
    }

    async process(musicBank) {
        const sourceDir = path.join(this._environment.assetDirectory, musicBank.dir);
        const name = musicBank.name;
        
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
            N64LibUltraMusicBankProcessor._checkOutputPaths([ctrlFilePath, tblFilePath, sbkFilePath, jsonFilePath]);

            const output = JSON.parse(fs.readFileSync(jsonFilePath, {encoding: "utf8"}));

            const instrumentBankPath = path.join(this._environment.outputDirectory, `${name}.instrumentbank`);
            SoundBank.writeFile(instrumentBankPath, ctrlFilePath, tblFilePath, 0);
            let index = -1;
            this._environment.assetBundle.withoutDefiningAssets(() => {
                index = this._environment.assetBundle.addFile(instrumentBankPath, name);
            });

            const sbkFileStats = fs.statSync(sbkFilePath);

            const musicBankHeader = Buffer.alloc(12);
            musicBankHeader.writeUInt32BE(output.length, 0);
            musicBankHeader.writeUInt32BE(index, 4);
            musicBankHeader.writeUInt32BE(sbkFileStats.size, 8);

            const musicBankPath = path.join(this._environment.outputDirectory, `${name}.musicbank`);
            const musicBankFile = fs.openSync(musicBankPath, "w");
            fs.writeSync(musicBankFile, musicBankHeader);
            fs.writeSync(musicBankFile, fs.readFileSync(sbkFilePath));
            fs.closeSync(musicBankFile);

            const files = fs.readdirSync(sourceDir);
            AudioHeader.writeMusicBankHeader(files, name, this._environment.includeDirectory);

            await this._environment.assetBundle.addMusicBank(musicBankPath, name);
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

module.exports = N64LibUltraMusicBankProcessor;
