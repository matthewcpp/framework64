const Pipeline = require("../pipeline/Pipeline");
const prepareDesktopShaders = require("./PrepareDesktopShaders");
const prepareWindowsDlls = require("./PrepareWindowsDlls");

const fse = require("fs-extra");
const path = require("path");
const rimraf = require("rimraf");

async function prepareAssets(assetManifest, assetDirectory, platform, platformBuildDir, gameName) {
	const gameBinDirectory = path.join(platformBuildDir,  "bin", gameName);
	const assetOutputDirectory = path.join(gameBinDirectory, "assets");

    if (fse.existsSync(assetOutputDirectory)) {
        rimraf.sync(assetOutputDirectory);
    }

    fse.ensureDirSync(assetOutputDirectory);

    await Pipeline.prepareAssets(assetManifest, assetDirectory, platform, assetOutputDirectory);

    const shouldPrepareShaders = platform.toLowerCase() === "desktop";
    const shouldPrepareDlls = process.platform === "win32" && platform.toLowerCase() === "desktop";

    if (shouldPrepareShaders) {
        const shaderDestDir = path.join(gameBinDirectory, "glsl");
        prepareDesktopShaders(shaderDestDir);
    }

    if (shouldPrepareDlls) {
        prepareWindowsDlls(platformBuildDir, gameBinDirectory);
    }
}

module.exports = prepareAssets;