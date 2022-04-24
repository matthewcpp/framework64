const { program } = require("commander");
const fse = require("fs-extra");
const rimraf = require("rimraf");

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

    await prepareAssets(assetManifest, assetDirectory, platform, platformBuildDir, target);
}