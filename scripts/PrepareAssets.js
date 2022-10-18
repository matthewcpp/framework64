const Pipeline = require("../pipeline/Pipeline");
const prepareDesktopShaders = require("./PrepareDesktopShaders");
const prepareWindowsDlls = require("./PrepareWindowsDlls");

const fse = require("fs-extra");
const path = require("path");
const rimraf = require("rimraf");

async function prepareAssets(assetManifest, assetDirectory, platform, platformBuildDir, gameName, pluginPath) {
	const gameBinDirectory = path.join(platformBuildDir,  "bin", gameName);
	const assetOutputDirectory = path.join(gameBinDirectory, "assets");
    const assetIncludeDirectory = path.join(assetOutputDirectory, "include", "assets");

    if (fse.existsSync(assetOutputDirectory)) {
        rimraf.sync(assetOutputDirectory);
    }

    fse.ensureDirSync(assetIncludeDirectory);

    await Pipeline.prepareAssets(assetManifest, assetDirectory, platform, assetOutputDirectory, pluginPath);

    const shouldPrepareShaders = platform.toLowerCase() === "desktop";
    

    if (shouldPrepareShaders) {
        const shaderDestDir = path.join(gameBinDirectory, "glsl");
        prepareDesktopShaders(shaderDestDir);
    }

    // It seems this is no longer necessary with VCPKG f688b2e29676bfa3a02ead4b7096c589f8af8180 (AUG 30, 2022)
    /*
    const shouldPrepareDlls = process.platform === "win32" && platform.toLowerCase() === "desktop";
    if (shouldPrepareDlls) {
        prepareWindowsDlls(platformBuildDir, gameBinDirectory);
    }
    */
}

module.exports = prepareAssets;