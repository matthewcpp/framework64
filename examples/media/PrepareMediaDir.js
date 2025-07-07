const Pipeline = require("../../pipeline/Pipeline");

const { program } = require("commander");

const fse = require("fs-extra");
const path = require("path");

program
    .name("Prepare media Dir")
    .version("1.0.0")
    .description("Prepare framework64 Assets");

program
    .argument("<platform>")
    .argument("<outputDirectory>")
    .action(prepareDir);

program.parse();

async function prepareDir(platform, mediaDirName) {
    const assetManifestFile = path.join(__dirname, "media_assets.json");
    const assetDirectory = path.resolve(path.join(__dirname, "..", "..", "assets"));
    const outputDirectory = path.resolve(path.join(__dirname, "..", "..", "build_" + platform, "bin", "media", mediaDirName), );

    console.log(`>>> Prepare Media Example Dir: ${outputDirectory}`);
    Pipeline.prepareAssets(assetManifestFile, assetDirectory, platform, outputDirectory, null);
}