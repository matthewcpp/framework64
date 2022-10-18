const { program } = require("commander");
const fse = require("fs-extra");

const path = require("path");

const prepareAssets = require("./PrepareAssets");

program
    .name("Prepare Example Assets")
    .version("1.0.0")
    .description("Prepares assets for a framework64 Example");

program
    .option("-p, --plugin <path>", "path to plugin configuration script");

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
    const options = program.opts();
    const pluginPath = (!!options.plugin) ? path.join(gameDirectory, options.plugin) : null;

    // read game name from package manifest if not specified
    if (!target) {
        const packageJsonPath = path.join(gameDirectory, "package.json");
        const packageJson = JSON.parse(fse.readFileSync(packageJsonPath, {encoding: "utf-8"}));
        target = packageJson.name;
        console.log(`No target specified. using default target: ${packageJson.name}`);
    }

    if (platform.toLowerCase() === "n64") {
        purgeCompiledAssetData(platformBuildDir, target)
    }

    await prepareAssets(assetManifest, assetDirectory, platform, platformBuildDir, target, pluginPath);
}

/** 
 * This function is needed because the compiler will need to regenerate the packed asset data when there is a change. 
*/
function purgeCompiledAssetData(platformBuildDir, targetName){
    const compiledDataPath = path.join(platformBuildDir, "src", "CMakeFiles", `${targetName}.dir`, "asm", "asset_data.s.obj");
    
    if (fse.existsSync(compiledDataPath)) {
        console.log(`Purging compiled asset data file: ${compiledDataPath}`);
        fse.unlinkSync(compiledDataPath)
    }
}