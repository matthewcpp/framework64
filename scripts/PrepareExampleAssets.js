const { program } = require("commander");

const path = require("path");
const fse = require("fs-extra");
const rimraf = require("rimraf");

const prepareAssets = require("./PrepareAssets");


program
    .name("Prepare Example Assets")
    .version("1.0.0")
    .description("Prepares assets for a framework64 Example");

program
    .argument("<example>")
    .argument("<platform>")
    .action(prepareExampleAssets);

program.parse();

async function prepareExampleAssets(example, platform) {
    if (example === "all")
        return prepareAllExampleAssets(platform);
    else
        return prepareExample(example, platform);
}

async function prepareAllExampleAssets(platform) {
    const examplesDirectory = path.resolve(__dirname, "..", "examples");
    const dirContents = fse.readdirSync(examplesDirectory, {withFileTypes: true});

    for(const item of dirContents) {
        if (!item.isDirectory)
            continue;
        
        // check if the folder is an example by looking for assets.json
        const assetsFile = path.join(examplesDirectory, item.name, "assets.json");
        if (fse.existsSync(assetsFile)) {
            await prepareExample(item.name, platform);
        }
    };
}

async function prepareExample(example, platform) {
    console.log("Preparing assets for example: ", example);
    const exampleDirectory = path.resolve(__dirname, "..", "examples", example);
    if (!fse.existsSync(exampleDirectory)) {
        console.error(`Unable to locate example: ${example}.  Expected path: ${exampleDirectory}`);
        process.exit(1);
    }

    const exampleManifestFile = path.join(exampleDirectory, "assets.json");
    if (!fse.existsSync(exampleManifestFile)){
        console.error(`Unable to locate asset manifest file at: ${exampleManifestFile}`);
        process.exit(1);
    }

    const assetDirectory = path.resolve(__dirname, "..", "assets");
    const platformBuildDir = path.resolve(__dirname, "..", `build_${platform}`);

    await prepareAssets(exampleManifestFile, assetDirectory, platform, platformBuildDir, example);
}