const AssetBundle = require("../AssetBundle");

const fs = require("fs");
const path = require("path");


class DesktopAssetBundle extends AssetBundle {
    _outputDirectory;

    constructor(outputDirectory) {
        super();
        this._outputDirectory = outputDirectory;
    }

    _addAsset(assetType, assetPath, assetName = null) {
        // Note: the desktop asset database expects paths relative to the output directory
        if (path.isAbsolute(assetPath)) {
            assetPath = path.relative(this._outputDirectory, assetPath);
        }

        return super._addAsset(assetType, assetPath, assetName)
    }

    /** Desktop Asset bundle simply reads a text file which maps asset Id's to path names to load. */
    writeAssetBundle(bundlePath) {
        const assetBundle = fs.openSync(bundlePath, "w");
        fs.writeSync(assetBundle, `${this.entries.length}\n`);
        for (const asset of this.entries) {
            fs.writeSync(assetBundle, `${asset.id} ${asset.path}\n`);
        }
        fs.closeSync(assetBundle)
    }
}

module.exports = DesktopAssetBundle;
