const Pipeline = require("../pipeline/Pipeline");

const path = require("path");
const fse = require("fs-extra");

async function prepreBuiltinAssets(folder, name, platform) {
    if (name.toLowerCase() == 'all') {
        return prepareAllBuiltinAssets(folder, platform);
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

    const gameBinDirectory = path.join(platformBuildDir,  "bin", name);
    const gameBuildDirectory = path.join(platformBuildDir, folder, name, "CMakeFiles", `${name}.dir`);

    const pluginManifestPath = path.join(targetDirectory, "pipeline", "plugins.json");
    await Pipeline.prepareAssets(manifestFile, assetDirectory, platform, gameBuildDirectory, gameBinDirectory, fse.existsSync(pluginManifestPath) ? pluginManifestPath: null);
}

async function prepareAllBuiltinAssets(topLevelDir, platform) {
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