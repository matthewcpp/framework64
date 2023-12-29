const fs = require("fs");

class AssetEntry {
    // the file that will be written into the bundle
    sourceFile;

    // the name of this asset
    name;

    // the type of this asset
    assetType;

    constructor(name, sourceFile, assetType) {
        this.sourceFile = sourceFile;
        this.name = name;
        this.assetType = assetType;
    }
}

class AssetBundle {
    assetEntries = [];

    addImage(name, sourceFile) {
        this.assetEntries.push(new AssetEntry(name, sourceFile, "image"));
    }

    addMesh(name, sourceFile) {
        this.assetEntries.push(new AssetEntry(name, sourceFile, "mesh"));
    }

    write(bundlePath, headerPath) {
        const bundleFile = fs.openSync(bundlePath, "w");
        const headerFile = fs.openSync(headerPath, "w");

        const keyShift = BigInt(32);
        const alignmentBuffer = Buffer.alloc(1);
        let bufferOffset = 0;
        fs.writeSync(headerFile, "#pragma once\n\n");

        for (const assetEntry of this.assetEntries) {
            const data = fs.readFileSync(assetEntry.sourceFile);

            // this is here because of N64 DMA alignment read requirements
            if ((bufferOffset & 0x1) !== 0) {
                fs.writeSync(bundleFile, alignmentBuffer);
                bufferOffset += 1;
            }

            fs.writeSync(bundleFile, data);

            const key = (BigInt(bufferOffset) << keyShift) | BigInt(data.length);
            fs.writeSync(headerFile, `#define FW64_ASSET_${assetEntry.assetType}_${assetEntry.name} ${key}\n`);
            bufferOffset += data.length;
        }

        fs.closeSync(bundleFile);
        fs.closeSync(headerFile);
    }
}

module.exports = AssetBundle
