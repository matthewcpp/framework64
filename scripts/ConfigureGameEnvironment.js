const configureEnvironment = require("./ConfigureEnvironment");
const { program } = require("commander");
const path = require("path");

program
    .name("Configure Game Environment")
    .version("1.0.0")
    .description("Configures a framework64 based game repo for development");

program
    .argument("<platform>")
    .action(configureGameEnvironment);

function configureGameEnvironment(platform) {
    configureEnvironment(platform, path.resolve(__dirname, "..", "..", ".."));
}

program.parse();