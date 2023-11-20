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
    if (example === "all")
        return prepareAllExampleAssets(platform);
    else
        return prepreBuiltinAssets("examples", example, platform);
}

async function prepareAllExampleAssets(platform) {
    const examplesDirectory = path.resolve(__dirname, "..", "examples");
    const dirContents = fse.readdirSync(examplesDirectory, {withFileTypes: true});

    for(const item of dirContents) {
        if (!item.isDirectory)
            continue;
        
        // check if the folder is an example by looking for assets.json
        const assetsFile = path.join(examplesDirectory, item.name, "assets.json");
        if (fse.existsSync(assetsFile)) {
            await prepreBuiltinAssets("examples", item.name, platform);
        }
    };
}
