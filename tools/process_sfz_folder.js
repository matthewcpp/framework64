const fs = require("fs");
const path = require("path");

if (process.argv.length < 3) {
    console.log("Usage: node process_sfz_folder /path/to/soundfont_dir [percussion_file.sfz]");
    process.exit(1);
}

const sfzExportFolder = process.argv[2];

const stats = fs.statSync(sfzExportFolder);
if (!stats.isDirectory()) {
    console.log(`${sfzExportFolder} is not a directory`);
    process.exit(1);
}

let percussionFile = null;
if (process.argv.length >= 4) {
    percussionFile = process.argv[3];
    const percussionFilePath = path.join(sfzExportFolder, percussionFile);

    if (!fs.existsSync(percussionFilePath)) {
        console.log(`Percussion file does not exist: ${percussionFilePath}`);
        process.exit(1);
    }
}

const multiSfzFilePath = path.join(sfzExportFolder, `_${path.basename(sfzExportFolder)}.sfz`);

if (fs.existsSync(multiSfzFilePath)) {
    fs.unlinkSync(multiSfzFilePath);
}

const files = fs.readdirSync(sfzExportFolder, {withFileTypes: true});

const multiSfzFile = fs.openSync(multiSfzFilePath, "w");

fs.writeSync(multiSfzFile, "<bank>\n\n");

if (percussionFile !== null) {
    fs.writeSync(multiSfzFile, "<percussion>\n");
    fs.writeSync(multiSfzFile, `instrument=${percussionFile}\n\n`);
}


for (const file of files) {
    if (!file.isFile())
        continue;

    if (path.extname(file.name) !== ".sfz")
        continue;

    //if (file.name === percussionFile)
    //    continue;

    const programNumber = parseInt(file.name.substr(0, file.name.indexOf('_'))) + 1;

    fs.writeSync(multiSfzFile, "<instrument>\n");
    fs.writeSync(multiSfzFile, `program_number=${programNumber}\n`);
    fs.writeSync(multiSfzFile, `instrument=${file.name}\n\n`);
}

fs.closeSync(multiSfzFile)
