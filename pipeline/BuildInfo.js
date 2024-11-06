const fs = require("fs");
const path = require("path");

/** The goal is for this class to no longer be a singleton */
class BuildInfo {
    _platformName;
    _is64Bit;
    _isLittleEndian;

    static _current = null;

    static get current() {
        return BuildInfo._current;
    }

    constructor(platformName, is64Bit, isLittleEndian) {
        this._platformName = platformName;
        this._is64Bit = is64Bit;
        this._isLittleEndian = isLittleEndian;
    }

    get is64BitBuild() {
        return this._is64Bit;
    }

    get sizeOfVoidPtr() {
        return this._is64Bit ? 8 : 4;
    }

    get platform() {
        return this._platformName;
    }

    get isLittleEndian() {
        return this._isLittleEndian;
    }
}

module.exports = BuildInfo;