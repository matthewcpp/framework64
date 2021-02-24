const fs = require("fs");
const path = require("path");
const util = require('util');
const exec = util.promisify(require('child_process').exec);

const DockerContainer = "matthewcpp/framework64-audio";

const envelope = `
envelope SE_Envelope
{
\tattackTime = 0;
\tattackVolume = 127;
\tdecayTime = 4000000;
\tdecayVolume = 0;
\treleaseTime = 200000;
}\n\n
`;

function sound(index) {
    return `
sound Sound_${index}
{
\tuse("./${index}.aifc");
\tpan = 64;
\tvolume = 127;
\tenvelope = SE_Envelope;
}
`;
}

function soundList(count) {
    let list = "";

    for (let i = 0; i < count; i++) {
        list += `\tsound = Sound_${i}\n`;
    }

    return list;
}

function instrument(count) {
    return `
instrument SE_Instrument
{
\tvolume = 127;
\tpan = 64;

${soundList(count)}
}
`;
}

const bank = `
bank SE_Bank
{
\tsampleRate = 44100;
\tinstrument[0] = SE_Instrument;
}\n\n
`;

function writeInstrumentFile(soundBank) {
    const filePath = path.join(soundBank.directory, `${soundBank.name}.ins`);
    const file = fs.openSync(filePath, "w");

    fs.writeSync(file, envelope);

    for (let i = 0; i < soundBank.fileCount; i++) {
        fs.writeSync(file, sound(i));
    }

    fs.writeSync(file, instrument(soundBank.fileCount));
    fs.writeSync(file, bank);

    fs.closeSync(file);
}

async function runDocker(soundBank) {
    const { stdout } = await exec(`docker run --rm -v ${soundBank.directory}:/workspace -u $(id -u):$(id -g) ${DockerContainer} sh /make_sound_effect_bank.sh`);
    console.log(stdout);

}

async function writeSoundBank(soundBank) {
    writeInstrumentFile(soundBank);
    await runDocker(soundBank);
}

module.exports = {
    writeSoundBank: writeSoundBank
}