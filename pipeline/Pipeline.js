#!/usr/bin/env node

const { program } = require('commander');
const rimraf = require("rimraf");

const fs = require("fs");

async function prepareAssets(manifestFile, assetDirectory, platform, outputDirectory) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));

    switch (platform) {
        case "n64":
            const prepareN64 = require("./n64/Prepare");
            await prepareN64(manifest, assetDirectory, outputDirectory);
            break;

        case "desktop":
            const prepareDesktop = require("./desktop/Prepare");
            await prepareDesktop(manifest, assetDirectory, outputDirectory);
            break;

        default:
            throw new Error(`Unsupported platform: ${manifest.platform}`);
    }
}

module.exports = {
    prepareAssets: prepareAssets
};

if (require.main === module) {
    require("./Main")();
    main();
}