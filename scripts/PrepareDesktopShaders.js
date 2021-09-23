const fse = require("fs-extra");

async function copyDesktopShaders(srcDir, destDir) {
    await fse.ensureDir(destDir);
    fse.copySync(srcDir, destDir);
}

if (require.main === module) {
    copyDesktopShaders(process.argv[2], process.argv[3]);
}

module.exports = copyDesktopShaders;

