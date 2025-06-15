const Pipeline = require("../../pipeline/Pipeline");

const { program } = require("commander");

const fse = require("fs-extra");

program
    .name("Prepare media Dir")
    .version("1.0.0")
    .description("Prepare framework64 Assets");

program
    // .argument("<platform>")
    // .argument("<assetManifestFile>")
    // .argument("<assetDirectory>")
    .argument("<outputDirectory>")
    .action(prepareDir);

program.parse();

function prepareDir(outputDirectory) {
    console.log(">>> Prepare Media Dir: " + outputDirectory);
}