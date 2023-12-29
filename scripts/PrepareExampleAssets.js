const { program } = require("commander");
const path = require("path");
const fse = require("fs-extra");

const prepreBuiltinAssets = require("./PrepareBuiltinAssets");

program
    .name("Prepare Example Assets")
    .version("1.0.0")
    .description("Prepares assets for a framework64 Example");

program
    .argument("<example>")
    .argument("<platform>")
    .action(prepareExampleAssets);

program.parse();

async function prepareExampleAssets(example, platform) {
    return prepreBuiltinAssets("examples", example, platform);
}
