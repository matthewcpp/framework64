const prepareAssets = require("./PrepareAssets");

const path = require("path");
const fse = require("fs-extra");
const { __esModule } = require("opentype.js");

async function prepreBuiltinAssets(folder, name, platform) {
    console.log("Preparing assets for: ", name);
    const targetDirectory = path.resolve(__dirname, "..", folder, name);

    if (!fse.existsSync(targetDirectory)) {
        console.error(`Unable to locate: ${name}.  Expected path: ${targetDirectory}`);
        process.exit(1);
    }

    const manifestFile = path.join(targetDirectory, "assets.json");
    if (!fse.existsSync(manifestFile)){
        console.error(`Unable to locate asset manifest file at: ${manifestFile}`);
        process.exit(1);
    }

    const assetDirectory = path.resolve(__dirname, "..", "assets");
    const platformBuildDir = path.resolve(__dirname, "..", `build_${platform}`);

    if (platform.toLowerCase() === "n64") {
        purgeCompiledAssetData(folder, platformBuildDir, name)
    }

    await prepareAssets(manifestFile, assetDirectory, platform, platformBuildDir, name);
}

/** 
 * This function is needed because the compiler will need to regenerate the packed asset data when there is a change. 
*/
function purgeCompiledAssetData(folder, platformBuildDir, targetName){
    const compiledDataPath = path.join(platformBuildDir, folder, targetName, "CMakeFiles", `${targetName}.dir`, "asm", "asset_data.s.obj");
    
    if (fse.existsSync(compiledDataPath)) {
        console.log(`Purging compiled asset data file: ${compiledDataPath}`);
        fse.unlinkSync(compiledDataPath)
    }
}

module.exports = prepreBuiltinAssets;