const fs = require("fs");
const path = require("path");
const Util = require("../Util");

// if we have a plugin that can process this file then defer to the plugin, otherwise just copy it over
async function processFile(fileJson, bundle, baseDirectory, outputDirectory, includeDirectory, plugins, environment) {
    const ext = path.extname(fileJson.src);

    if (plugins.has(ext)) {
        const plugin = plugins.get(ext);
        const outputFile = await plugin.process(fileJson, baseDirectory, outputDirectory, includeDirectory, environment)

        if (!!outputFile) {
            await bundle.add(outputFile, "file"); // n64 specific

            // TODO: Use this when both platforms use bundle class
            // const assetName = Util.safeDefineName(path.basename(outputFile, path.extname(outputFile)));
            // bundle.addFile(assetName, outputFile);
        }
    }
    else {
        const sourceFile = path.join(baseDirectory, rawPath);
        const destFile = path.join(outputDirectory, rawPath);
        fs.copyFileSync(sourceFile, destFile);
    }
}

module.exports = processFile;
