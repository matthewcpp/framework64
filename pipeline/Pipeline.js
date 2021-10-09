#!/usr/bin/env node

const { program } = require('commander');
const rimraf = require("rimraf");

const fs = require("fs");
const fse = require("fs-extra");
const path = require("path");

const prepare = require("./Prepare");

const supportedPlatforms = new Set(["n64", "desktop"]);

async function main() {
    let assetManifestPath = null;
    let outputDirectoryPath = null;
    let buildPlatform = null;

    program.version("0.1.0");

    program.arguments("<manifest> <platform> <outDir>");
    program.option("-f, --force", "force asset creation in existing directory");

    program.description("command", {
        manifest: "path to asset manifest file",
        platform: "the platform that assets will be prepared for",
        outDir: "path to output directory"
    });

    program.action((manifest, platform, outDir) => {
        assetManifestPath = path.resolve(manifest);
        buildPlatform = platform;
        outputDirectoryPath = path.resolve(outDir);
    });

    program.parse(process.argv);

    if (!supportedPlatforms.has(buildPlatform)) {
        console.log(`Unknown platform: ${buildPlatform}`);
        process.exit(1);
    }

    if (!fs.existsSync(assetManifestPath)) {
        console.log(`manifest file: ${assetManifestPath} does not exist.`);
        process.exit(1);
    }

    if (fs.existsSync(outputDirectoryPath)) {
        if (!program.force) {
            console.log(`specify -f or --force to build assets into existing directory.`);
            process.exit(1);
        }

        rimraf.sync(outputDirectoryPath);
    }

    await fse.ensureDir(outputDirectoryPath);

    await prepare(assetManifestPath, buildPlatform, outputDirectoryPath);
}

if (require.main === module) {
    main();
}
