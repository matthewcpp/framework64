const fs = require("fs");
const prepareN64 = require("./n64/Prepare");

async function prepare(manifestFile, outputDirectory) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));

    if (!manifest.platform) {
        throw new Error("Manifest file does not specify a platform");
    }

    switch (manifest.platform) {
        case "n64":
            const prepareN64 = require("./n64/Prepare");
            await prepareN64(manifest, manifestFile, outputDirectory);
            break;

        case "desktop":
            const prepareDesktop = require("./desktop/Prepare");
            await prepareDesktop(manifest, manifestFile, outputDirectory);
            break;

        default:
            throw new Error(`Unsupported platform: ${manifest.platform}`);
    }
}

module.exports = prepare;