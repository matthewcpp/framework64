const Util = require("./Util");

const fs = require("fs");

class AssetBundleEntry {
    type;
    id;
    path;
    name;
    define;

    constructor(type, id, path, name, define) {
        this.type = type;
        this.id = id;
        this.path = path;
        this.name = name;
        this.define = define;
    }
};

/** 
 * The asset Bundle interface defines a generic class for storing game assets and writing header files.
 * Each platform can this bundle and perform any platform specific logic needed when adding each asset type.
 * This class also provides common logic for writing the main asset header.
 * */
class AssetBundle {
    /** 
     * Controls wheter assets added to the bundle will be written into the main header file.
     * This could be used to control the visibility of assets with a transient dependnecies of other assets and may not be suited for loading individually.
     * */
    defineAssets = true;

    entries = [];

    /** Enumeration for all asset types that can be loaded from the asset database interface*/
    static AssetType = {
        AnimationData: "animationdata",
        File: "file",
        Font: "font",
        Image: "image",
        Mesh: "mesh",
        MusicBank: "musicbank",
        Scene: "scene",
        SkinnedMesh: "skinnedmesh",
        SoundBank: "soundbank",
    }

    addAnimationData(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.AnimationData, assetPath, assetName);
    }

    addFile(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.File, assetPath, assetName);
    }

    addFont(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.Font, assetPath, assetName);
    }

    addImage(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.Image, assetPath, assetName);
    }

    addMesh(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.Mesh, assetPath, assetName);
    }

    addMusicBank(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.MusicBank, assetPath, assetName);
    }

    addScene(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.Scene, assetPath, assetName);
    }

    addSkinnedMesh(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.SkinnedMesh, assetPath, assetName);
    }

    addSoundBank(assetPath, assetName = null) {
        return this._addAsset(AssetBundle.AssetType.SoundBank, assetPath, assetName);
    }

    withoutDefiningAssets(func) {
        this.defineAssets = false;
        func();
        this.defineAssets = true;
    }

    _addAsset(assetType, assetPath, assetName = null) {
        throw new Error("Dervied asset bundle should override the addAsset method");
    }

    /** Protected method for adding an asset into the bundle. All dervied classes should call this from their own AddAsset function*/
    _addAssetBase(assetType, assetId, assetPath, assetName) {
        this.entries.push(new AssetBundleEntry(assetType, assetId, assetPath, Util.safeDefineName(assetName), this.defineAssets));
        
        return assetId;
    }

    writeHeader(headerPath) {
        const file = fs.openSync(headerPath, "w");
        fs.writeSync(file, "#pragma once\n\n");

        for (const entry of this.entries) {
            if (!entry.define) {
                continue;
            }

            fs.writeSync(file, `#define FW64_ASSET_${entry.type}_${entry.name} ${entry.id}\n`);
        }

        fs.writeSync(file,`#define FW64_ASSET_COUNT ${this.entries.length}\n\n`);

        fs.closeSync(file);
    }

    writeManifest(manifestPath) {
        const file = fs.openSync(manifestPath, "w");

        for (const entry of this.entries) {
            fs.writeSync(file, `${entry.id}: ${entry.path}\n`);
        }

        fs.closeSync(file);
    }
}

module.exports = AssetBundle
