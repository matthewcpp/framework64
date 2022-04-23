const { program } = require("commander");

const path = require("path");
const fse = require("fs-extra");
const rimraf = require("rimraf");

const prepareAssets = require("../pipeline/Prepare");
const prepareDesktopShaders = require("./PrepareDesktopShaders");
const prepareWindowsDlls = require("./PrepareWindowsDlls");

async function prepareExampleAssets(example, platform) {
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
    const exampleOutDirectory = path.join(platformBuildDir, "bin", example)
    const outputDirectory = path.join(exampleOutDirectory, "assets");

    if (fse.existsSync(outputDirectory)) {
        rimraf.sync(outputDirectory);
    }

    fse.ensureDirSync(outputDirectory);
    await prepareAssets(exampleManifestFile, assetDirectory, platform, outputDirectory);

    const shouldPrepareShaders = platform.toLowerCase() === "desktop";
    const shouldPrepareDlls = process.platform === "win32" && platform.toLowerCase() === "desktop";

    if (shouldPrepareShaders) {
        const exampleShaderDir = path.join(exampleOutDirectory, "glsl");
        prepareDesktopShaders(exampleShaderDir);
    }

    if (shouldPrepareDlls) {
        prepareWindowsDlls(platformBuildDir, exampleOutDirectory);
    }
}

program
    .name("Prepare Example Assets")
    .version("1.0.0")
    .description("Prepares assets for a framework64 Example");

program
    .argument("<example>")
    .argument("<platform>")
    .action(prepareExampleAssets);

program.parse();