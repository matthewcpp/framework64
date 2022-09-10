const { program } = require("commander");
const fse = require("fs-extra");

const path = require("path");

const prepareAssets = require("./PrepareAssets");

program
    .name("Prepare Example Assets")
    .version("1.0.0")
    .description("Prepares assets for a framework64 Example");

program
    .argument("<platform>")
    .argument("[target]")
    .action(prepareGameAssets);

program.parse();

async function prepareGameAssets(platform, target) {
    const gameDirectory = path.resolve(__dirname, "..", "..", "..");
    const assetDirectory = path.join(gameDirectory, "assets");
    const assetManifest = path.join(assetDirectory, "assets.json");
    const platformBuildDir = path.join(gameDirectory, `build_${platform}`);


    // read game name from package manifest if not specified
    if (!target) {
        const packageJsonPath = path.join(gameDirectory, "package.json");
        const packageJson = JSON.parse(fse.readFileSync(packageJsonPath, {encoding: "utf-8"}));
        target = packageJson.name;
        console.log(`No target specified. using default target: ${packageJson.name}`);
    }

    console.log(assetManifest, assetDirectory, platform, platformBuildDir, target);

    if (platform.toLowerCase() === "n64") {
        purgeAssetDataAssemblyFile(platformBuildDir, target)
    }

    await prepareAssets(assetManifest, assetDirectory, platform, platformBuildDir, target);
}

/** 
 * This function is needed because the compiler will need to regenerate the packed asset data when there is a change. 
 * Touching this file does not seem to case the data to be regenerated, therefore we just nuke it when the assets are updated
 * This results in the need for the cmake configure step to be re-ran but in general that is quick.
*/
function purgeAssetDataAssemblyFile(platformBuildDir) {
    const assetDataAsm = path.join(platformBuildDir, "src", "asm", "asset_data.s");

    if (fse.existsSync(assetDataAsm)) {
        console.log(`Purging existing asset data assembly file: ${assetDataAsm}`);
        fse.unlinkSync(assetDataAsm)
    }
}