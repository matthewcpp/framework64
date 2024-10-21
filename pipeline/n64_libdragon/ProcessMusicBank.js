const AudioHeader = require("../AudioHeader");

const fs = require("fs");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

async function processMusicBank(musicBank, dfsAssets, baseDirectory, outputDirectory, includeDirectory) {
    const musicBankName = (!!musicBank.name) ? musicBank.name : path.basename(musicBank.dir);
    const sourceDir = path.join(baseDirectory, musicBank.dir);
    const destDirName = Util.safeDefineName(musicBankName);
    const destDir = path.join(outputDirectory, destDirName);

    // const dockerArgs = [
    //     "run", "--rm",
    //     "-v", `${assetDirectory}:${dockerMappedInDir}`,
    //     "-v", `${outputDirectory}:${dockerMappedOutDir}`,
    //     "matthewcpp/framework64-n64-libdragon",
    //     mkspritePath,
    //     "--output", dockerMappedOutDir,
    //     "--compress", "0",
    //     "-f", format,
    //     sourcePath
    // ];

    // await execFile("docker", dockerArgs);

    // const files = fs.readdirSync(sourceDir);
}