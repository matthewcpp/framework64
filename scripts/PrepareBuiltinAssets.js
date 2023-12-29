const prepareAssets = require("./PrepareAssets");

const path = require("path");
const fse = require("fs-extra");

async function prepreBuiltinAssets(folder, name, platform) {
    if (name.toLowerCase() == 'all') {
        return prepareAllExampleAssets(folder, platform);
    }

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

    if (platform.toLowerCase() === "n64_libultra") {
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

async function prepareAllExampleAssets(topLevelDir, platform) {
    const examplesDirectory = path.resolve(__dirname, "..", topLevelDir);
    const dirContents = fse.readdirSync(examplesDirectory, {withFileTypes: true});

    for(const item of dirContents) {
        if (!item.isDirectory)
            continue;
        
        // check if the folder is an example by looking for assets.json
        const assetsFile = path.join(examplesDirectory, item.name, "assets.json");
        if (fse.existsSync(assetsFile)) {
            await prepreBuiltinAssets(topLevelDir, item.name, platform);
        }
    };
}

module.exports = prepreBuiltinAssets;