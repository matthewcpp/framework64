const { program } = require("commander");

const prepreBuiltinAssets = require("./PrepareBuiltinAssets");

program
    .name("Prepare Tool Assets")
    .version("1.0.0")
    .description("Prepares assets for a framework64 Tool");

program
    .argument("<tool>")
    .argument("<platform>")
    .action(prepareToolAssets);

program.parse();

async function prepareToolAssets(tool, platform) {
        return prepreBuiltinAssets("tools", tool, platform);
}
