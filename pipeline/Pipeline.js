#!/usr/bin/env node

const { program } = require('commander');
const rimraf = require("rimraf");

const fs = require("fs-extra");
const path = require("path");
const Util = require("./Util")

async function prepareAssets(manifestFile, assetDirectory, platform, outputDirectory, pluginManifest) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    platform = platform.toLowerCase();

    const pluginMap = loadPlugins(pluginManifest);

    // ensure that required asset folders are setup for downstream processors
    const assetIncludeDirectory = Util.assetIncludeDirectory(outputDirectory);
    fs.ensureDirSync(assetIncludeDirectory);

    switch (platform) {
        case "n64_libultra":
            const processN64 = require("./n64/Process");
            await processN64(manifest, assetDirectory, outputDirectory, plugins);
            break;

        case "desktop":
            const processDesktop = require("./desktop/Process");
            await processDesktop(manifest, assetDirectory, outputDirectory, plugins);
            break;

        default:
            throw new Error(`Unsupported platform: ${manifest.platform}`);
    }
}

function loadPlugins(pluginManifestPath) {
    const pluginMap = new Map();

    if (!pluginManifestPath) {
        return pluginMap;
    }

    if (!fs.existsSync(pluginManifestPath)) {
        throw new Error(`Plugin manifest does not exist: ${pluginManifestPath}`);
    }

    const pluginManifest = JSON.parse(fs.readFileSync(pluginManifestPath, {encoding: "utf8"}));
    const manifestDir = path.dirname(pluginManifestPath);

    if (!Object.hasOwn(pluginManifest, "plugins")) {
        return pluginMap;
    }

    for (let i = 0; i < pluginManifest.plugins.length; i++) {
        const plugInfo = pluginManifest.plugins[i];

        if (!Object.hasOwn(plugInfo, "src")) {
            throw new Error(`Plugin ${i} does not specify a 'src' attribute`);
        }

        if (!Object.hasOwn(plugInfo, "extensions") || plugInfo.extensions.length === 0) {
            throw new Error(`Plugin does not declare any file extensions to process: ${plugInfo.src}`);
        }

        const pluginPath = path.join(manifestDir, plugInfo.src);
        if (!fs.existsSync(pluginPath)) {
            throw new Error(`Plugin does not exist: ${pluginPath}`);
        }

        const pluginClass = require(pluginPath);

        if (typeof(pluginClass) !== "function") {
            throw new Error(`Plugin script should export a single class: ${plugInfo.src}`);
        }

        const pluginInstance = new pluginClass();

        for (const extension of plugInfo.extensions) {
            if (pluginMap.has(extension)) {
                throw new Error(`Multiple plugins registered for extension: ${extension}`)
            }

            pluginMap.set(extension, pluginInstance);
        }
    }

    return pluginMap;
}

module.exports = {
    prepareAssets: prepareAssets
};

if (require.main === module) {
    require("../scripts/RunPipeline")();
    main();
}