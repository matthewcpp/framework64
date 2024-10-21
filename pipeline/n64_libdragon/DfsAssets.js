const fs = require("fs");
const path = require("path");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const mkdfsPath = "/opt/libdragon/bin/mkdfs";
const dockerMappedInDir = "/tmp/in";
const dockerMappedOutDir = "/tmp/out";
const dfpsOutPath = "/tmp/out/assets.dfs"

class DfsAssets {
    _defineAssets = true;
    _nextAssetId = 0;
    assetDefines = [];
    manifestEntries = [];

    writeHeader(headerPath) {
        const file = fs.openSync(headerPath, "w");
        fs.writeSync(file, "#pragma once\n\n");

        for (const define of this.assetDefines) {
            fs.writeSync(file, define);
        }

        fs.writeSync(file,`#define FW64_ASSET_COUNT ${this.assetDefines.length}\n\n`);

        fs.closeSync(file);
    }

    writeManifest(manifestPath) {
        const file = fs.openSync(manifestPath, "w");

        for (const manifestEntry of this.manifestEntries) {
            fs.writeSync(file, `${manifestEntry[0]}: ${manifestEntry[1]}\n`);
        }

        fs.closeSync(file);
    }

    _addAsset(type, assetPath, assetName) {
        const assetId = this._nextAssetId++;

        this.manifestEntries.push([assetId, assetPath]);

        if (this._defineAssets) {
            this.assetDefines.push(`#define FW64_ASSET_${type}_${assetName} "rom:/${assetId}"\n`)
        }
        
        return assetId;
    }

    addImage(imagePath, imageName) {
        return this._addAsset("image", imagePath, imageName);
    }

    addMesh(meshPath, meshName) {
        return this._addAsset("mesh", meshPath, meshName);
    }

    addSkinnedMesh(skinnedMeshPath, skinnedMeshName) {
        return this._addAsset("skinnedmesh", skinnedMeshPath, skinnedMeshName);
    }

    addAnimationData(animationPath, animationName) {
        return this._addAsset("skinnedmesh", animationPath, animationName);
    }

    addFile(filePath, fileName) {
        return this._addAsset("file", filePath, fileName);
    }

    writePlaceholderFile(dfsSrcDir) {
        const placeholderPath = path.join(dfsSrcDir, "empty");
        const file = fs.openSync(placeholderPath, "w");
        fs.writeSync(file, "empty");
        fs.closeSync(file);
    }

    set defineAssetsInHeader(define) {
        this._defineAssets = define;
    }

    async makeBundle(dfsSrcDir, outputDir) {
        // console.log(`DfsAssets: ${dfsSrcDir} , ${outputDir}`);

        const runingInDocker = Object.hasOwn(process.env, "FW64_N64_LIBDRAGON_DOCKER_CONTAINER");

        if (runingInDocker) {
            throw new Error("docker not supported, yet");
        }

        // if bundle is empty need to write a file there
        if (this.assetDefines.length === 0) {
            this.writePlaceholderFile(dfsSrcDir);
        }


        const dockerArgs = [
            "run", "--rm",
            "-v", `${dfsSrcDir}:${dockerMappedInDir}`,
            "-v", `${outputDir}:${dockerMappedOutDir}`,
            "matthewcpp/framework64-n64-libdragon",
            mkdfsPath,
            dfpsOutPath,
            dockerMappedInDir
        ];
    
        await execFile("docker", dockerArgs);
    }
}

module.exports = DfsAssets;