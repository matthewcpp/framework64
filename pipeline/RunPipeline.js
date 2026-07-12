/* This is a generic script that external clients can use to run the asset pipeline.
   It is currently used by the data link asset service to convert assets on the fly.
*/
const Pipeline = require("./Pipeline");

const { program } = require("commander");

const fse = require("fs-extra");

program
    .name("Prepare Assets")
    .version("1.0.0")
    .description("Prepare framework64 Assets");

program
    .argument("<platform>")
    .argument("<assetManifestFile>")
    .argument("<assetDirectory>")
    .argument("<outputDirectory>")
    .argument("[pluginManifestFile]")
    .action(runPipeline);

program.parse();

async function runPipeline(platform, assetManifestFile, assetDirectory, outputDirectory, pluginManifestFile = null) {
    if (!fse.existsSync(assetManifestFile)) {
        console.log(`Asset manifest file: ${assetManifestFile} does not exist.`);
        process.exit(1);
    }

    if (!fse.existsSync(assetDirectory)) {
        console.log(`Asset directory: ${assetDirectory} does not exist.`);
        process.exit(1);
    }

    if (pluginManifestFile !== null && !fse.existsSync(pluginManifestFile)) {
        console.log(`Plugin manifest file: ${pluginManifestFile} does not exist.`);
        process.exit(1);
    }

    console.log("Running framework64 Asset Pipeline");
    console.log(`    Platform: ${platform}`);
    console.log(`    Asset manifest File: ${assetManifestFile}`);
    console.log(`    Asset Directory: ${assetDirectory}`);
    console.log(`    Output Directory: ${outputDirectory}`);

    if (pluginManifestFile !== null ) {
        console.log(`    Plugin manifest file: ${pluginManifestFile}`);
    }

    await Pipeline.prepareAssets(assetManifestFile, assetDirectory, platform, outputDirectory, pluginManifestFile);
}
