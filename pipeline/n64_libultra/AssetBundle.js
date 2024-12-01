const AssetBundle = require("../AssetBundle");

const fs = require("fs");
const path = require("path");

class N64LibUltraAssetBundle extends AssetBundle {

    _addAsset(assetType, assetPath, assetName = null) {
        if (assetName === null) {
            assetName = path.basename(assetPath, path.extname(assetPath));
        }

        return this._addAssetBase(assetType, this.entries.length, assetPath, assetName)
    }

    writeArchive(archivePath) {
        const archive = fs.openSync(archivePath, 'w');

        // setup the header which will be placed at the beginning of the archive
        const headerBuffer = Buffer.alloc(4 + (this.entries.length * 4));
        let headerBufferOffset = headerBuffer.writeUInt32BE(this.entries.length);
        fs.writeSync(archive, headerBuffer); // reserve header size in file

        const binaryBuffer = Buffer.alloc(4); //used as a scratch for binary data

        let archiveOffset = headerBuffer.length;
        this.entries.forEach((entry) => {
            // record offset for the current item in the archive header buffer
            headerBufferOffset = headerBuffer.writeUInt32BE(archiveOffset, headerBufferOffset);
            const data = fs.readFileSync(entry.path);

            binaryBuffer.writeUInt32BE(data.length);
            fs.writeSync(archive, binaryBuffer);
            fs.writeSync(archive, data);

            archiveOffset += 4 + data.length;

            //each file should be 2 byte aligned to help with reads
            if ((archiveOffset & 0x1) !== 0) {
                binaryBuffer.writeUInt8(0);
                fs.writeSync(archive, binaryBuffer, 0, 1);
                archiveOffset += 1;
            }

        })

        //write the completed header into the archive
        fs.writeSync(archive, headerBuffer, 0, headerBuffer.length, 0);
        fs.closeSync(archive);
    }
}


module.exports = N64LibUltraAssetBundle;