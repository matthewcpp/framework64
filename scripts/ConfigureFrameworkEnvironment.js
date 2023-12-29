const configureEnvironment = require("./ConfigureEnvironment");
const { program } = require("commander");
const path = require("path");

program
    .name("Configure Development Environment")
    .version("1.0.0")
    .description("Configures the framework64 repo for development");

program
    .argument("<platform>")
    .action(configureFrameworkEnvironment);

function configureFrameworkEnvironment(platform) {
    configureEnvironment(platform, path.resolve(__dirname, ".."));
}

program.parse();