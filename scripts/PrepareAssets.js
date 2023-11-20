const Pipeline = require("../pipeline/Pipeline");
const BuildInfo = require("../pipeline/BuildInfo");
const prepareDesktopShaders = require("./PrepareDesktopShaders");
const prepareWindowsDlls = require("./PrepareWindowsDlls");

const fse = require("fs-extra");
const path = require("path");
const rimraf = require("rimraf");

async function prepareAssets(assetManifest, assetDirectory, platform, platformBuildDir, gameName, pluginDirPath) {
	const gameBinDirectory = path.join(platformBuildDir,  "bin", gameName);
	const assetOutputDirectory = path.join(gameBinDirectory, "assets");

    if (fse.existsSync(assetOutputDirectory)) {
        rimraf.sync(assetOutputDirectory);
    }

    platform = platform.toLowerCase();

    switch(platform) {
        case "desktop": {
            BuildInfo.initDesktop(platformBuildDir);
            break;
        }

        case "n64": {
            BuildInfo.initN64();
            break;
        }

        default: {
            throw new Error(`Build Info not configured for: ${platform}`);
        }
    }
    

    await Pipeline.prepareAssets(assetManifest, assetDirectory, platform, assetOutputDirectory, pluginDirPath);

    if (platform === "desktop") {
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