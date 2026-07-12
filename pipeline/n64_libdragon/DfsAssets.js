const AssetBundle = require("../AssetBundle");

const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const mkdfsPath = "/opt/libdragon/bin/mkdfs";

class DfsAssets extends AssetBundle{
    _currentOffset = 0;

    constructor() {
        super();
    }

    /** The asset id is the bitwise AND of two 32 bit values:
     *  The high bits are the current offset into the file
     *  The low bits are the size of the asset
     *  This needs to be kept in sync with libdragon_asset_database.c
     */
    _getAssetId(assetType, assetPath, assetName) {
        const stats = fs.statSync(assetPath);
        const id = (BigInt(this._currentOffset) << BigInt(32)) | BigInt(stats.size);
        this._currentOffset += stats.size;

        return id;
    }

    /** this name needs be to kept in sync with ASSET_BUNDLE_PATH in libdragon_asset_database.c */
    static _assetBundleFileName = "assets.bundle";

    /** Write all the assets into the bundle which resides in the dfs dir
     *  Once the bundle has been assembles we invoke makedfs on it.
     */
    async makeBundle(dfsDir, outputFile) {
        const bundleFilePath = path.join(dfsDir, DfsAssets._assetBundleFileName);
        const file = fs.openSync(bundleFilePath, 'w');

        let bytesWritten = BigInt(0);

        for (const entry of this.entries) {
            const buffer = fs.readFileSync(entry.path);
            const expectedOffset = BigInt(entry.id) >> BigInt(32); // high 32 bits is the offset;
            const expectedSize = entry.id & BigInt(0xFFFFFFFF); // low 32 bits is the size

            if (expectedOffset !== bytesWritten) {
                throw new Error(`Unexpected bundle byte offset for ${entry.path}. Expected: ${expectedOffset} Actual: ${bytesWritten}`);
            }

            if (expectedSize !== BigInt(buffer.length)) {
                throw new Error(`Unexpected file size for ${entry.path}. Expected: ${expectedOffset} Actual: ${buffer.length}`);
            }
            
            bytesWritten += BigInt(fs.writeSync(file, buffer));
        }

        fs.closeSync(file);

        // create dfs file containing our asset bundle to be included in ROM.
        const mkdfsArgs = [
            outputFile,
            dfsDir
        ];
    
        const result = await execFile(mkdfsPath, mkdfsArgs);
        console.log(result.stdout);
        console.log(result.stderr);
    }
}

module.exports = DfsAssets;