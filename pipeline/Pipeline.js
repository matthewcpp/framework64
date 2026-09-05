#!/usr/bin/env node

const runInDocker = require("./RunInDocker");
const Util = require("./Util");
const Environment = require("./Environment");
const Plugins = require("./Plugins");

const rimraf = require("rimraf");

const fse = require("fs-extra");
const path = require("path");
const { glMatrix } = require("gl-matrix");

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
    glMatrix.setMatrixArrayType(Array);

    platform = platform.toLowerCase();

    if (await runInDocker(manifestFile, assetDirectory, platform, outputDirectory, pluginManifest)) {
        return;
    }

    if (!fse.existsSync(manifestFile)) {
        throw new Error(`Manifest file does not exist: ${manifestFile}`);
    }

    if (fse.existsSync(outputDirectory)) {
        rimraf.sync(outputDirectory);
    }

    const plugins = loadPlugins(pluginManifest);

    // ensure that required asset folders are setup for downstream processors
    const assetIncludeDirectory = Util.assetIncludeDirectory(outputDirectory);
    fse.ensureDirSync(assetIncludeDirectory);

    switch (platform) {
        case "n64_libultra": {
            const processN64 = require("./n64_libultra/Process");
            await processN64(manifestFile, assetDirectory, outputDirectory, plugins);
            break;
        }

        case "n64_libdragon":
            const processN64Libdragon = require("./n64_libdragon/Process");
            await processN64Libdragon(manifestFile, assetDirectory, outputDirectory, plugins);
            break;

        case "desktop":{
            const processDesktop = require("./desktop/Process").processDesktop;
            await processDesktop(manifestFile, assetDirectory, outputDirectory, plugins);
            break;
        }

        case "web":{
            // note: right now only support 32 bit wasm is supported.
            const processWeb = require("./web/Process");
            await processWeb(manifestFile, assetDirectory, outputDirectory, plugins);
            break;
        }

        default:
            throw new Error(`Unsupported platform: ${platform}`);
    }
}

function loadPlugins(pluginManifestPath) {
    const plugins = new Plugins();

    if (!pluginManifestPath) {
        return plugins;
    }

    pluginManifestPath = path.resolve(pluginManifestPath);

    if (!fse.existsSync(pluginManifestPath)) {
        throw new Error(`Plugin manifest does not exist: ${pluginManifestPath}`);
    }

    plugins.load(pluginManifestPath);

    return plugins;
}

module.exports = {
    prepareAssets: prepareAssets
};