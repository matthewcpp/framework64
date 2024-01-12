const fs = require("fs");
const path = require("path");

class BuildInfo {
    platformName;
    is64Bit;
    isLittleEndian;

    constructor(platformName, is64Bit, isLittleEndian) {
        this.platformName = platformName;
        this.is64Bit = is64Bit;
        this.isLittleEndian = isLittleEndian;
    }

    static buildInfo = null;

    static get is64BitBuild() {
        return BuildInfo.buildInfo.is64Bit;
    }

    static get sizeOfVoidPtr() {
        return BuildInfo.buildInfo.is64Bit ? 8 : 4;
    }

    static get platform() {
        return BuildInfo.buildInfo.platformName;
    }

    static get isLittleEndian() {
        return BuildInfo.buildInfo.isLittleEndian;
    }

    /* platform specific configuration functions go below */

    static initDesktop(desktopBuildDirectory) {
        const buildInfoFile = path.join(desktopBuildDirectory, "build_info.json");

        if (!fs.existsSync(buildInfoFile)) {
            throw new Error(`Unable to load build info file at: ${buildInfoFile}. Ensure that you have configured a build with CMake before running the asset pipeline.`);
        }

        const buildInfoJson = JSON.parse(fs.readFileSync(buildInfoFile, {encoding: "utf-8"}));
        const x64Targets = ["arm64-osx", "x64-osx", "x64-windows"];

        BuildInfo.buildInfo = new BuildInfo(
            buildInfoJson.target, 
            x64Targets.indexOf(buildInfoJson.config) >= 0,
            true);
    }

    static initN64Libultra() {
        BuildInfo.buildInfo = new BuildInfo("n64_libultra", false, false);
    }

    static initN64Libdragon() {
        BuildInfo.buildInfo = new BuildInfo("n64_libdragon", false, false);
    }
}

module.exports = BuildInfo;