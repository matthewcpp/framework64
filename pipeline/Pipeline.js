#!/usr/bin/env node

const { program } = require('commander');
const rimraf = require("rimraf");

const fs = require("fs");
const path = require("path");
const Plugins = require("./Plugins");

async function prepareAssets(manifestFile, assetDirectory, platform, outputDirectory, pluginScript) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    platform = platform.toLowerCase();

    const plugins = new Plugins(loadPlugin(pluginScript, platform));

    switch (platform) {
        case "n64":
            const prepareN64 = require("./n64/Prepare");
            await prepareN64(manifest, assetDirectory, outputDirectory, plugins);
            break;

        case "desktop":
            const prepareDesktop = require("./desktop/Prepare");
            await prepareDesktop(manifest, assetDirectory, outputDirectory, plugins);
            break;

        default:
            throw new Error(`Unsupported platform: ${manifest.platform}`);
    }
}

function loadPlugin(pluginScript, platform) {
    if (!pluginScript) {
        return null;
    }

    if (!fs.existsSync(pluginScript)) {
        throw new Error(`Unable to load plugin script at: ${pluginScript}`);
    }

    const pluginCreateFunc = require(pluginScript);
    if (typeof(pluginCreateFunc) !== "function") {
        throw new Error("Plugin script should export a single function");
    }

    const pluginObject = pluginCreateFunc(platform);

    if (typeof(pluginObject) !== "object") {
        throw new Error("Plugin function should return a single plugin object");
    }

    return pluginObject;
}

module.exports = {
    prepareAssets: prepareAssets
};

if (require.main === module) {
    require("./Main")();
    main();
}