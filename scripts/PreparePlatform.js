const fse = require("fs-extra");
const path = require("path");

/**
 * @param platform the platform that is currently being built
 * @param gameBuildDirectory the path to the directory where all compiled code will be built to
 * @param gameBinDirectory the output directory for this game's binary files
 */
function preparePlatform(platform, gameBuildDirectory, gameBinDirectory) {
    switch(platform) {
        case "desktop":
            const shaderDestDir = path.join(gameBinDirectory, "glsl");
            prepareDesktopShaders(shaderDestDir);
        break;

        case "n64_libultra":
            purgeCompiledAssetData(gameBuildDirectory);
        break;
    }
}

async function prepareDesktopShaders(destDir) {
    console.log("Preparing Desktop Shaders");
    const shaderDir = path.resolve(__dirname, "..", "src", "framework64", "desktop", "glsl");

    if (!fse.existsSync(shaderDir)) {
        throw new Error(`Unable to locate shader directory: ${shaderDir}`);
    }

    await fse.ensureDir(destDir);

    console.log(`Copy: ${shaderDir} -> ${destDir}`);
    fse.copySync(shaderDir, destDir);
}

/** This function is needed because the compiler will need to regenerate the packed asset data when there is a change. */
function purgeCompiledAssetData(gameBuildDirectory){
    const compiledDataPath = path.join(gameBuildDirectory, "asm", "asset_data.s.obj");

    if (fse.existsSync(compiledDataPath)) {
        console.log(`Purging compiled asset data file: ${compiledDataPath}`);
        fse.unlinkSync(compiledDataPath)
    }
}

module.exports = preparePlatform;