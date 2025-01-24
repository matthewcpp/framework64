const AssetBundle = require("../AssetBundle");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const mkdfsPath = "/opt/libdragon/bin/mkdfs";
const dockerMappedInDir = "/tmp/in";
const dockerMappedOutDir = "/tmp/out";
const dfpsOutPath = "/tmp/out/assets.dfs"

class DfsAssets extends AssetBundle{
    _addAsset(assetType, assetPath, assetName = null) {
        if (assetName === null) {
            assetName = path.basename(assetPath, path.extname(assetPath));
        }

        return this._addAssetBase(assetType, this.entries.length, assetPath, assetName)
    }

    _writePlaceholderFile(dfsSrcDir) {
        const placeholderPath = path.join(dfsSrcDir, "empty");
        const file = fs.openSync(placeholderPath, "w");
        fs.writeSync(file, "empty");
        fs.closeSync(file);
    }

    async makeBundle(dfsSrcDir, outputFile) {
        // if bundle is empty need to write a placeholder file to avoid errors
        if (this.entries.length === 0) {
            this._writePlaceholderFile(dfsSrcDir);
        }

        const mkdfsArgs = [
            outputFile,
            dfsSrcDir
        ];
    
        const result = await execFile(mkdfsPath, mkdfsArgs);
        // console.log(result.stdout);
        // console.log(result.stderr);
    }
}

module.exports = DfsAssets;