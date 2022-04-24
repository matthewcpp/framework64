const { program } = require("commander");
const fs = require("fs");

const path = require("path");

program
    .name("Configure Framework64 Enviornment")
    .version("1.0.0")
    .description("Performs and platform specific configuration to the game directory");

program
    .argument("<platform>")
    .action(configureEnvironment);

program.parse();

function configureEnvironment(platform) {
    switch (platform.toLowerCase()) {
        case "n64-modernsdk":
            configureN64ModernSdk();
            break;

        default:
            console.error(`Unsupported platform specified: ${platform}`);
            process.exit(1);
    }
}

function configureN64ModernSdk() {
    console.log("Configuring for: N64 Modern SDK");

    const modernSdkConfigDir = path.resolve(__dirname, "..", "config", "n64.modernsdk");
    const devcontainerSrcFile = path.join(modernSdkConfigDir, "n64.modernsdk.devcontainer.json");
    const devcontainerDestFile = path.resolve(".devcontainer.json");
    console.log(`Copy: ${devcontainerSrcFile} --> ${devcontainerDestFile}`);
    fs.copyFileSync(devcontainerSrcFile, devcontainerDestFile);
}