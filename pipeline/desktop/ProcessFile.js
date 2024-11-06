const fs = require("fs");
const path = require("path");

// if we have a plugin that can process this file then defer to the plugin, otherwise just copy it over
async function processFile(fileJson, environment, plugins) {
    const ext = path.extname(fileJson.src);

    if (plugins.has(ext)) {
        const plugin = plugins.get(ext);
        await plugin.process(fileJson, environment);
    }
    else {
        const sourceFile = path.join(environment.assetDirectory, fileJson.src);
        const destFile = path.join(environment.outputDirectory, path.basename(fileJson.src));
        fs.copyFileSync(sourceFile, destFile);

        const assetName = path.basename(fileJson.src, path.extname(fileJson.src));
        bundle.addFile(destFile, assetName);
    }
}

module.exports = processFile;
