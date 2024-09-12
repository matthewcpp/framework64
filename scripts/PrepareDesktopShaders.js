const fse = require("fs-extra");
const path = require("path");

async function prepareDesktopShaders(destDir) {
    console.log("Preparing Desktop Shaders");
    const shaderDir = path.resolve(__dirname, "..", "src", "framework64", "desktop", "glsl");

    if (!fse.existsSync(shaderDir)) {
        throw new Error(`Unable to locate shader directory: ${shaderDir}`);
    }

    await fse.ensureDir(destDir);

    console.log(`Copy: ${shaderDir} -> ${destDir}`);
    fse.copySync(shaderDir, destDir);
}

if (require.main === module) {
    copyDesktopShaders(process.argv[2]);
}

module.exports = prepareDesktopShaders;

