#!/usr/bin/env node

const Util = require("./Util");

const { program } = require('commander');
const rimraf = require("rimraf");

const fse = require("fs-extra");
const path = require("path");

/**
 * 
 * @param manifestFile the path to the asset manifest json file
 * @param assetDirectory the root path of the folder containing all assets
 * @param platform the platform that is currently being built
 * @param platformBuildDirectory the root build directory for this platform
 * @param outputDirectory the output directory for this game's assets
 * @param pluginManifest the path pointing to the plugin manifest for this build
 */
async function prepareAssets(manifestFile, assetDirectory, platform, outputDirectory, pluginManifest) {
    if (!fse.existsSync(manifestFile)) {
        throw new Error(`Manifest file does not exist: ${manifestFile}`);
    }

    if (fse.existsSync(outputDirectory)) {
        rimraf.sync(outputDirectory);
    }

    platform = platform.toLowerCase();

    const pluginMap = loadPlugins(pluginManifest);

    // ensure that required asset folders are setup for downstream processors
    const assetIncludeDirectory = Util.assetIncludeDirectory(outputDirectory);
    fse.ensureDirSync(assetIncludeDirectory);

    switch (platform) {
        case "n64_libultra":
            //purgeCompiledAssetData(gameBuildDirectory)
            const processN64 = require("./n64_libultra/Process");
            await processN64(manifestFile, assetDirectory, outputDirectory, pluginMap);
        break;

        case "desktop":
            const processDesktop = require("./desktop/Process");
            await processDesktop(manifestFile, assetDirectory, outputDirectory, pluginMap);
        break;

        default:
            throw new Error(`Unsupported platform: ${platform}`);
    }
}

function loadPlugins(pluginManifestPath) {
    const pluginMap = new Map();

    if (!pluginManifestPath) {
        return pluginMap;
    }

    pluginManifestPath = path.resolve(pluginManifestPath);

    if (!fse.existsSync(pluginManifestPath)) {
        throw new Error(`Plugin manifest does not exist: ${pluginManifestPath}`);
    }

    const pluginManifest = JSON.parse(fse.readFileSync(pluginManifestPath, {encoding: "utf8"}));
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
        if (!fse.existsSync(pluginPath)) {
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

        console.log(`Loaded plugin: ${plugInfo.src}`);
    }

    return pluginMap;
}

module.exports = {
    prepareAssets: prepareAssets
};