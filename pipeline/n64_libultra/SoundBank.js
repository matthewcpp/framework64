const fs = require("fs");

function writeFile(soundBankPath, ctlFilePath, tblFilePath, fileCount) {
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

module.exports = {
    writeFile: writeFile
}