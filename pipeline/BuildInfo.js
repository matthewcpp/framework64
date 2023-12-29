const fs = require("fs");
const path = require("path");

class BuildInfo {
    static buildInfo = null;
    static x64Targets = ["arm64-osx", "x64-osx", "x64-windows"];

    static get is64BitBuild() {
        return BuildInfo.x64Targets.indexOf(this.buildInfo.config) >= 0;
    }

    static get sizeOfVoidPtr() {
        return BuildInfo.is64BitBuild ? 8 : 4;
    }

    static get platform() {
        return this.buildInfo.target;
    }

    /* platform specific configuration functions go below */

    static initDesktop(desktopBuildDirectory) {
        const buildInfoFile = path.join(desktopBuildDirectory, "build_info.json");

        if (!fs.existsSync(buildInfoFile)) {
            throw new Error(`Unable to load build info file at: ${buildInfoFile}. Ensure that you have configured a build with CMake before running the asset pipeline.`);
        }

        BuildInfo.buildInfo = JSON.parse(fs.readFileSync(buildInfoFile, {encoding: "utf-8"}));
    }

    static initN64Libultra() {
        BuildInfo.buildInfo = {
            target: "n64_libultra"
        };
    }

    static initN64Libdragon() {
        BuildInfo.buildInfo = {
            target: "n64_libdragon"
        };
    }
}

module.exports = BuildInfo;