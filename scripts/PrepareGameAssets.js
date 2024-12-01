const Pipeline = require("../pipeline/Pipeline");
const preparePlatform = require("./PreparePlatform");

const { program } = require("commander");
const fse = require("fs-extra");

const path = require("path");

program
    .name("Prepare Game Assets")
    .version("1.0.0")
    .description("Prepares assets for a framework64 Game");

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

    const gameBinDirectory = path.join(platformBuildDir,  "bin", target);
    const gameBuildDirectory = path.join(platformBuildDir, "src", "CMakeFiles", `${target}.dir`);
    const outputDirectory = path.join(gameBinDirectory, "assets");
    const pluginManifestPath = path.join(gameDirectory, "pipeline", "plugins.json");

    // read game name from package manifest if not specified
    if (!target) {
        const packageJsonPath = path.join(gameDirectory, "package.json");
        const packageJson = JSON.parse(fse.readFileSync(packageJsonPath, {encoding: "utf-8"}));
        target = packageJson.name;
        console.log(`No target specified. using default target: ${packageJson.name}`);
    }

    preparePlatform(platform, gameBuildDirectory, gameBinDirectory);
    await Pipeline.prepareAssets(assetManifest, assetDirectory, platform, outputDirectory, fse.existsSync(pluginManifestPath) ? pluginManifestPath: null);
}

