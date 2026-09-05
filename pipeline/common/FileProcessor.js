const fs = require("fs");
const path = require("path");

class BasicFileProcessor {
    _environment;
    _plugins;

    constructor(environment, plugins) {
        this._environment = environment;
        this._plugins = plugins;
    }

    async process(fileJson) {
        const ext = path.extname(fileJson.src);

        // if we have a plugin that can process this file then defer to the plugin, otherwise just copy it over
        if (this._plugins.filePlugins.has(ext)) {
            const plugin = this._plugins.filePlugins.get(ext);
            await plugin.process(fileJson, this._environment);
        }
        else {
            const sourceFile = path.join(this._environment.assetDirectory, fileJson.src);
            const destFile = path.join(this._environment.outputDirectory, path.basename(fileJson.src));
            fs.copyFileSync(sourceFile, destFile);

            const assetName = path.basename(fileJson.src, path.extname(fileJson.src));
            this._environment.assetBundle.addFile(destFile, assetName);
        }
    }
};



module.exports = BasicFileProcessor;
