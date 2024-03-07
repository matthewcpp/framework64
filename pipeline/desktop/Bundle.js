const Util = require("../Util");

const fse = require("fs-extra");
const path = require("path");

class Asset {
    id;
    path;

    constructor(id, path) {
        this.id = id;
        this.path = path;
    }
}

class Bundle {
    _nextAssetId = 1
    _headerFile = 0;

    _outputDirectory = null;
    _assets = []

    constructor(outputDirectory, includeDirectory) {
        this._outputDirectory = outputDirectory;
        this._initHeaderFile(includeDirectory);
    }

    _initHeaderFile(includeDirectory) {
        const headerPath = path.join(includeDirectory, "assets.h");
        this._headerFile = fse.openSync(headerPath, "w");
        fse.writeSync(this._headerFile, "#pragma once\n\n");
    }

    addAnimationData(name, animationDataPath) {
        const assetId = ++this._nextAssetId;
        this._assets.push(new Asset(assetId, animationDataPath));
        fse.writeSync(this._headerFile,`#define FW64_ASSET_animation_data_${name} ${assetId}\n`);

        return assetId;
    }

    addImage(name, imagePath) {
        const assetId = ++this._nextAssetId
        this._assets.push(new Asset(assetId, imagePath));
        fse.writeSync(this._headerFile,`#define FW64_ASSET_image_${name} ${assetId}\n`);

        return assetId;
    }

    addFont(name, fontPath) {
        const assetId = ++this._nextAssetId;
        this._assets.push(new Asset(assetId, fontPath));
        fse.writeSync(this._headerFile,`#define FW64_ASSET_font_${name} ${assetId}\n`);

        return assetId;
    }

    addMesh(meshName, assetPath) {
        const assetId = ++this._nextAssetId;
        const assetName = meshName;

        this._assets.push(new Asset(assetId, assetPath));
        fse.writeSync(this._headerFile,`#define FW64_ASSET_mesh_${assetName} ${assetId}\n`);

        return assetId;
    }

    addScene(sceneName, assetPath) {
        const assetId = ++this._nextAssetId;
        this._assets.push(new Asset(assetId, assetPath));

        fse.writeSync(this._headerFile,`#define FW64_ASSET_scene_${sceneName} ${assetId}\n`);

        return assetId;
    }

    addSkinnedMesh(name, assetPath) {
        const assetId = ++this._nextAssetId;
        this._assets.push(new Asset(assetId, assetPath));

        fse.writeSync(this._headerFile,`#define FW64_ASSET_skinnedmesh_${name} ${assetId}\n`);

        return assetId;
    }

    addSoundBank(name, assetDir) {
        const assetId = ++this._nextAssetId
        const assetName = name;

        this._assets.push(new Asset(assetId, assetDir));
        fse.writeSync(this._headerFile,`#define FW64_ASSET_soundbank_${assetName} ${assetId}\n`);

        return assetId;
    }

    addMusicBank(name, assetDir) {
        const assetId = ++this._nextAssetId;
        const assetName = name;

        this._assets.push(new Asset(assetId, assetDir));
        fse.writeSync(this._headerFile,`#define FW64_ASSET_musicbank_${assetName} ${assetId}\n`);

        return assetId;
    }

    addFile(name, assetPath) {
        const assetId = ++this._nextAssetId;
        const assetName = name;

        this._assets.push(new Asset(assetId, assetPath));
        fse.writeSync(this._headerFile,`#define FW64_ASSET_file_${assetName} ${assetId}\n`);

        return assetId;
    }

    finalize() {
        const assetCount = this._nextAssetId - 1;
        fse.writeSync(this._headerFile, `#define FW64_ASSET_COUNT ${assetCount}\n\n`);
        fse.closeSync(this._headerFile);


        const bundlePath = path.join(this._outputDirectory, "asset_bundle.txt");
        const assetBundle = fse.openSync(bundlePath, "w");
        fse.writeSync(assetBundle, `${this._assets.length}\n`);
        for (const asset of this._assets) {
            fse.writeSync(assetBundle, `${asset.id} ${asset.path}\n`);
        }
        fse.closeSync(assetBundle)
    }
}

module.exports = Bundle;
