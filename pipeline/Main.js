const fse = require("fs-extra");
const path = require("path");

const supportedPlatforms = new Set(["n64", "desktop"]);

async function pipelineMain() {
    let assetManifestPath = null;
    let assetDirectoryPath = null;
    let outputDirectoryPath = null;
    let buildPlatform = null;

    program.version("0.1.0");

    program.arguments("<manifest> <assetDir> <platform> <outDir>");
    program.option("-f, --force", "force asset creation in existing directory");

    program.description("command", {
        manifest: "path to asset manifest file",
        platform: "the platform that assets will be prepared for",
        outDir: "path to output directory"
    });

    program.action((manifest, assetDir, platform, outDir) => {
        assetManifestPath = path.resolve(manifest);
        assetDirectoryPath = path.resolve(assetDir);
        buildPlatform = platform.toLowerCase();
        outputDirectoryPath = path.resolve(outDir);
    });

    program.parse(process.argv);
    const options = program.opts();

    if (!supportedPlatforms.has(buildPlatform)) {
        console.log(`Unknown platform: ${buildPlatform}`);
        process.exit(1);
    }

    if (!fse.existsSync(assetManifestPath)) {
        console.log(`manifest file: ${assetManifestPath} does not exist.`);
        process.exit(1);
    }

    if (!fse.existsSync(assetDirectoryPath)) {
        console.log(`Asset directory: ${assetDirectoryPath} does not exist.`);
        process.exit(1);
    }

    if (fse.existsSync(outputDirectoryPath)) {
        if (!options.force) {
            console.log(`specify -f or --force to build assets into existing directory.`);
            process.exit(1);
        }

        rimraf.sync(outputDirectoryPath);
    }

    await prepareAssets(assetManifestPath, assetDirectoryPath, buildPlatform, outputDirectoryPath);
}