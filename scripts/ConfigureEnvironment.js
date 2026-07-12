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

        case "n64_libdragon":
            configureN64Libdragon(configDir, destDirectory);
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
    copyDevcontainerJson("n64_libultra", configDir, destDirectory);
}

function configureN64Libdragon(configDir, destDirectory) {
    console.log("Configuring for: N64 libdragon");
    copyDevcontainerJson("n64_libdragon", configDir, destDirectory);
}

function copyDevcontainerJson(platform, configDir, destDirectory) {
    const devcontainerSrcFile = path.join(configDir, platform, `${platform}.devcontainer.json`);
    const devcontainerDestFile = path.join(destDirectory, ".devcontainer.json");
    console.log(`Copy: ${devcontainerSrcFile} --> ${devcontainerDestFile}`);
    fs.copyFileSync(devcontainerSrcFile, devcontainerDestFile);
}

module.exports = configureEnvironment;