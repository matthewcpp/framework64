#!/usr/bin/env node

const { program } = require('commander');
const rimraf = require("rimraf");

const fs = require("fs");
const path = require("path");
const Plugins = require("./Plugins");

async function prepareAssets(manifestFile, assetDirectory, platform, outputDirectory, pluginDirPath) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    platform = platform.toLowerCase();

    const plugins = new Plugins(loadPlugins(pluginDirPath, platform));

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

function loadPlugins(pluginDir, platform) {
    if (!pluginDir) {
        return null;
    }

    if (!fs.existsSync(pluginDir)) {
        throw new Error(`Plugin directory does not exist: ${pluginDir}`);
    }

    const plugins = [];
    const entries = fs.readdirSync(pluginDir, {withFileTypes: true});
    for (const entry of entries) {
        if (!entry.isFile)
            continue;
        
        const pluginPath = path.join(pluginDir, entry.name);
        const pluginClass = require(pluginPath);

        if (typeof(pluginClass) !== "function") {
            throw new Error("Plugin script should export a single class");
        }

        plugins.push(new pluginClass());
    }

    return plugins;
}

module.exports = {
    prepareAssets: prepareAssets
};

if (require.main === module) {
    require("./Main")();
    main();
}