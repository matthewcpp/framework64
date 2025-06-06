const fs = require("fs");
const path = require("path");

function configureEnvironment(platform, destDirectory) {
    const configDir = path.resolve(__dirname, "..", "config");

    switch (platform) {
        case "desktop":
            configureDesktop(configDir, destDirectory);
            break;

        case "n64_libultra":
            configureN64Libultra(configDir, destDirectory);
            break;
    
        default:
            console.error(`Unsupported platform specified: ${platform}`);
            process.exit(1);
    }
}

function configureDesktop(configDir, destDirectory) {
    console.log("Configuring for: desktop");
    // Note: currently there is no desktop specific configuration that is needed and is included for parity
}

function configureN64Libultra(configDir, destDirectory) {
    console.log("Configuring for: N64 libultra");

    const devcontainerSrcFile = path.join(configDir, "n64_libultra", "n64_libultra.devcontainer.json");
    const devcontainerDestFile = path.join(destDirectory, ".devcontainer.json");
    console.log(`Copy: ${devcontainerSrcFile} --> ${devcontainerDestFile}`);
    fs.copyFileSync(devcontainerSrcFile, devcontainerDestFile);
}

module.exports = configureEnvironment;