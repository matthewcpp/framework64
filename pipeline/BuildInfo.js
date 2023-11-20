const fs = require("fs");
const path = require("path");

class BuildInfo {
    static buildInfo = null;
    static x64Targets = ["arm64-osx", "x64-osx", "x64-windows"];

    static initDesktop(desktopBuildDirectory) {
        const buildInfoFile = path.join(desktopBuildDirectory, "build_info.json");

        if (!fs.existsSync(buildInfoFile)) {
            throw new Error(`Unable to load build info file at: ${buildInfoFile}. Ensure that you have configured a build with CMake before running the asset pipeline.`);
        }

        BuildInfo.buildInfo = JSON.parse(fs.readFileSync(buildInfoFile, {encoding: "utf-8"}));
    }

    static initN64() {
        BuildInfo.buildInfo = {
            target: "n64"
        };
    }

    static get is64BitBuild() {
        return BuildInfo.x64Targets.indexOf(this.buildInfo.target) >= 0;
    }

    static get sizeOfVoidPtr() {
        return BuildInfo.is64BitBuild ? 8 : 4;
    }
}

module.exports = BuildInfo;