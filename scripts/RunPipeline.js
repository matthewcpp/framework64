const { program } = require("commander");

const Pipeline = require("../pipeline/Pipeline");

const path = require("path");
const fse = require("fs-extra");
const rimraf = require("rimraf");

const supportedPlatforms = new Set(["n64", "desktop"]);

program
    .name("Prepare Assets")
    .version("1.0.0")
    .description("Prepare framework64 Assets");

program
    .argument("<assetManifestFile>")
    .argument("<assetDirectory>")
    .argument("<outputDirectory>")
    .argument("<platform>")
    .action(runPipeline);

program.parse();

async function runPipeline(assetManifestFile, assetDirectory, outputDirectory, platform) {
    if (!supportedPlatforms.has(platform.toLowerCase())) {
        console.log(`Unknown platform: ${platform}`);
        process.exit(1);
    }

    if (!fse.existsSync(assetManifestFile)) {
        console.log(`manifest file: ${assetManifestFile} does not exist.`);
        process.exit(1);
    }

    if (!fse.existsSync(assetDirectory)) {
        console.log(`Asset directory: ${assetDirectory} does not exist.`);
        process.exit(1);
    }

    if (!fse.existsSync(outputDirectory)) {
        console.log(`Asset directory: ${outputDirectory} does not exist.`);
        process.exit(1);
    }

    await Pipeline.prepareAssets(assetManifestFile, assetDirectory, platform, outputDirectory, null);
}

