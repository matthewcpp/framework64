const Pipeline = require("../pipeline/Pipeline");
const preparePlatform = require("./PreparePlatform");

const path = require("path");
const fse = require("fs-extra");
const process = require("process");

async function prepreBuiltinAssets(folder, name, platform) {
    if (name.toLowerCase() == 'all') {
        return prepareAllBuiltinAssets(folder, platform);
    }

    console.log("Preparing assets for: ", name);

    // Note: When running in a fw64 dev container, the pipeline is mapped to its own container in order for
    // the container's node modules to be picked up. We need to account for that here.
    // This handles the case where we are trying to run / debug the asset pipeline from within the VS Code development container.
    const scriptsDirectory = Object.hasOwn(process.env, "FW64_DEV_CONTAINER") ?
        "/workspaces/framework64/scripts" : __dirname;
    const targetDirectory = path.resolve(scriptsDirectory, "..", folder, name);

    if (!fse.existsSync(targetDirectory)) {
        console.error(`Unable to locate: ${name}.  Expected path: ${targetDirectory}`);
        process.exit(1);
    }

    const manifestFile = path.join(targetDirectory, "assets.json");
    if (!fse.existsSync(manifestFile)){
        console.error(`Unable to locate asset manifest file at: ${manifestFile}`);
        process.exit(1);
    }

    const assetDirectory = path.resolve(scriptsDirectory, "..", "assets");
    const platformBuildDir = path.resolve(scriptsDirectory, "..", `build_${platform}`);

    const gameBinDirectory = path.join(platformBuildDir,  "bin", name);
    const gameBuildDirectory = path.join(platformBuildDir, folder, name, "CMakeFiles", `${name}.dir`);
    const outputDirectory = path.join(gameBinDirectory, "assets");

    const pluginManifestPath = path.join(targetDirectory, "pipeline", "plugins.json");

    preparePlatform(platform, gameBuildDirectory, gameBinDirectory);
    await Pipeline.prepareAssets(manifestFile, assetDirectory, platform, outputDirectory, fse.existsSync(pluginManifestPath) ? pluginManifestPath: null);
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