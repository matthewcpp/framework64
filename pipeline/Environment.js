const BuildInfo = require("./BuildInfo");
const WriteInterface = require("./WriteInterface");
const Writer = require("./WriteInterface");

class Environment {
    _writer;
    _assetBundle;
    _assetDirectory;
    _includeDirectory;
    _outputDirectory;
    _pipelineDirectory;

    constructor(assetBundle, assetDirectory, outputDirectory, includeDirectory, pipelineDirectory) {
        this._assetBundle = assetBundle;
        this._assetDirectory = assetDirectory;
        this._outputDirectory = outputDirectory;
        this._includeDirectory = includeDirectory;
        this._pipelineDirectory = pipelineDirectory;

        this._writer = BuildInfo.isLittleEndian ? WriteInterface.littleEndian() : WriteInterface.bigEndian();
    }

    /** Information about the current build  */
    get buildInfo() {
        return BuildInfo.buildInfo;
    }

    /** Asset bundle object that should be used to added assets during processing. */
    get assetBundle() {
        return this._assetBundle;
    }

    /** A write interface configured for the endianness of the current platform. */
    get binaryWriter() {
        return this._writer;
    }

    /** The root directory for game assets. */
    get assetDirectory() {
        return this._assetDirectory;
    }

    /** 
     * The directory that any header files related to assets should be placed in.
     * In the game source code, files placed in this directory can be accessed using #include "assets/<file>.h"
     */
    get includeDirectory() {
        return this._includeDirectory;
    }

    /** The directory that processed asset files should be written to  */
    get outputDirectory() {
        return this._outputDirectory;
    }

    /** The directory containing common pipeline files. */
    get pipelineDirectory() {
        return this._pipelineDirectory;
    }
}

module.exports = Environment;