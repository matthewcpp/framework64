const fs = require("fs");
const path = require("path");

if (process.argv.length < 3) {
    console.log("Usage: node ./ConfigureEnvironment.js <platform>")
    process.exit(1);
}

const platform = process.argv[2].toLowerCase();

switch (platform) {
    case "n64_modernsdk":
        configureN64ModernSdk();
        break;

    default:
        console.error(`Unsupported platform specified: ${platform}`);
        process.exit(1);
}

function configureN64ModernSdk() {
    console.log("Configuring for: N64 Modern SDK");

    const configDir = path.resolve(__dirname, "..", "config", "n64.modernsdk");
    const devcontainerSrcFile = path.join(configDir, "n64.modernsdk.devcontainer.json");
    const devcontainerDestFile = path.join(__dirname, "..", ".devcontainer.json");
    console.log(`Copy: ${devcontainerSrcFile} --> ${devcontainerDestFile}`);
    fs.copyFileSync(devcontainerSrcFile, devcontainerDestFile);
}
    console.log(`Copy: ${devcontainerSrcFile} --> ${devcontainerDestFile}`);
    fs.copyFileSync(devcontainerSrcFile, devcontainerDestFile);
}