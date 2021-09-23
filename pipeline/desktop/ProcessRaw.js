const fs = require("fs");
const path = require("path");

function processRaw(rawPath, bundle, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, rawPath);
    const destFile = path.join(outputDirectory, rawPath);

    fs.copyFileSync(sourceFile, destFile);

    const stats = fs.statSync(sourceFile);
    bundle.addRaw(rawPath, stats.size);
}

module.exports = processRaw;