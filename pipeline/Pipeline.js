#!/usr/bin/env node

const BuildInfo = require('./BuildInfo');
const Util = require("./Util");
// TODO: this needs to be moved into desktop
const prepareDesktopShaders = require("../scripts/PrepareDesktopShaders");

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
 * @param gameBuildDirectory the path to the directory where all compiled code will be built to
 * @param gameBinDirectory the output directory for this game's binary files
 * @param pluginManifest the path pointing to the plugin manifest for this build
 */
async function prepareAssets(manifestFile, assetDirectory, platform, platformBuildDirectory, gameBuildDirectory, gameBinDirectory, pluginManifest) {
    const outputDirectory = path.join(gameBinDirectory, "assets");

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
            BuildInfo._current = new BuildInfo(platform, false, false);
            purgeCompiledAssetData(gameBuildDirectory)
            const processN64 = require("./n64_libultra/Process");
            await processN64(manifestFile, assetDirectory, outputDirectory, pluginMap);
            break;

        case "desktop":
            BuildInfo._current = createDesktopBuildInfo(platformBuildDirectory);
            const processDesktop = require("./desktop/Process");
            await processDesktop(manifestFile, assetDirectory, outputDirectory, pluginMap);
            
            /// TODO: This should be moved into the desktop processing directory
            const shaderDestDir = path.join(gameBinDirectory, "glsl");
            prepareDesktopShaders(shaderDestDir);
            break;

        default:
            throw new Error(`Unsupported platform: ${manifest.platform}`);
    }
}

/** TODO: It would be good to move this into the desktop specific code*/
function createDesktopBuildInfo(desktopBuildDirectory) {
    const buildInfoFile = path.join(desktopBuildDirectory, "build_info.json");

    if (!fse.existsSync(buildInfoFile)) {
        throw new Error(`Unable to load build info file at: ${buildInfoFile}. Ensure that you have configured a build with CMake before running the asset pipeline.`);
    }

    const buildInfoJson = JSON.parse(fse.readFileSync(buildInfoFile, {encoding: "utf-8"}));
    const x64Targets = ["arm64-osx", "x64-osx", "x64-windows"];

    return new BuildInfo(
        buildInfoJson.target, 
        x64Targets.indexOf(buildInfoJson.config) >= 0,
        true);
}

/** 
 * This function is needed because the compiler will need to regenerate the packed asset data when there is a change.
 * TODO: this should be moved into n64_libulta directory
*/
function purgeCompiledAssetData(gameBuildDirectory){
    const compiledDataPath = path.join(gameBuildDirectory, "asm", "asset_data.s.obj");

    if (fse.existsSync(compiledDataPath)) {
        console.log(`Purging compiled asset data file: ${compiledDataPath}`);
        fse.unlinkSync(compiledDataPath)
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

if (require.main === module) {
    require("../scripts/RunPipeline")();
    main();
}