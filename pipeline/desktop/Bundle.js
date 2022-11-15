const sqlite3 = require('sqlite3').verbose();

const fse = require("fs-extra");
const path = require("path");

const Util = require("../Util")

class Bundle {
    _nextAssetId = 1
    _headerFile = 0;

    _db = null;

    constructor(outputDirectory, includeDirectory) {
        this._initDatabase(outputDirectory);
        this._initHeaderFile(includeDirectory);
    }
    

    _initHeaderFile(includeDirectory) {
        const headerPath = path.join(includeDirectory, "assets.h");
        this._headerFile = fse.openSync(headerPath, "w");
        fse.writeSync(this._headerFile, "#pragma once\n\n");
    }

    _initDatabase(outputDirectory) {
        const dbPath = path.join(outputDirectory, "assets.db");
        const db = new sqlite3.Database(dbPath);
        this._db = db;

        db.serialize(() => {
            db.run("CREATE TABLE images (assetId INTEGER PRIMARY KEY, path TEXT, hslices INTEGER, vslices INTEGER, indexMode INTEGER)");
            db.run("CREATE TABLE fonts (assetId INTEGER PRIMARY KEY, path TEXT, size INTEGER, tileWidth INTEGER, tileHeight INTEGER, glyphCount INTEGER, glyphData BLOB )");
            db.run("CREATE TABLE meshes (assetId INTEGER PRIMARY KEY, path TEXT, jointMap TEXT)");
            db.run("CREATE TABLE soundBanks (assetId INTEGER PRIMARY KEY, path TEXT, count INTEGER)");
            db.run("CREATE TABLE musicBanks (assetId INTEGER PRIMARY KEY, path TEXT, count INTEGER)");
            db.run("CREATE TABLE rawFiles (assetId INTEGER PRIMARY KEY, path TEXT, size INTEGER)");
            db.run("CREATE TABLE layerMaps (assetId INTEGER PRIMARY KEY, jsonIndex INTEGER, jsonStr TEXT)");
            db.run("CREATE TABLE scenes (assetId INTEGER PRIMARY KEY, path TEXT, sceneIndex INTEGER, layerMap INTEGER)");
            db.run("CREATE TABLE palettes (id INTEGER PRIMARY KEY AUTOINCREMENT, imageId INTEGER, path TEXT)");
        });
    }

    addAnimationData(animationDataPath) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(animationDataPath, path.extname(animationDataPath));

        this._db.run("INSERT INTO rawFiles VALUES (?, ?, ?)", [assetId, animationDataPath, 0]);
        fse.writeSync(this._headerFile,`#define FW64_ASSET_animation_data_${assetName} ${assetId}\n`);

        return assetId;
    }

    addImage(imagePath, imageName, hslices, vslices, indexMode) {
        const assetId = this._nextAssetId++;

        this._db.run("INSERT INTO images VALUES (?, ?, ?, ?, ?)", [assetId, imagePath, hslices, vslices, (!!indexMode) ? 1 : 0])
        fse.writeSync(this._headerFile,`#define FW64_ASSET_image_${imageName} ${assetId}\n`);

        return assetId;
    }

    addPalette(imageId, path) {
        this._db.run("INSERT INTO palettes (imageId, path) VALUES (?, ?)", [imageId, path]);
    }

    addFont(font, fontPath) {
        const assetId = this._nextAssetId++;

        this._db.run("INSERT INTO fonts VALUES (?, ?, ?, ?, ?, ?, ?)", [assetId, fontPath, font.size, font.tileWidth, font.tileHeight, font.glyphs.length, font.desktopGlyphBuffer]);
        fse.writeSync(this._headerFile,`#define FW64_ASSET_font_${font.name} ${assetId}\n`);

        return assetId;
    }

    addMesh(meshName, assetPath, jointMap) {
        const assetId = this._nextAssetId++;
        const assetName = meshName;

        this._db.run("INSERT INTO meshes VALUES (?, ?, ?)", [assetId, assetPath, jointMap ? JSON.stringify(jointMap): null]);
        fse.writeSync(this._headerFile,`#define FW64_ASSET_mesh_${assetName} ${assetId}\n`);

        return assetId;
    }

    addSoundBank(soundBank, files) {
        const assetId = this._nextAssetId++;
        const assetName = soundBank.name;

        this._db.run("INSERT INTO soundBanks VALUES (?, ?, ?)", [assetId, assetName, files.length]);
        fse.writeSync(this._headerFile,`#define FW64_ASSET_soundbank_${assetName} ${assetId}\n`);

        return assetId;
    }

    addMusicBank(musicBank, files) {
        const assetId = this._nextAssetId++;
        const assetName = musicBank.name;

        this._db.run("INSERT INTO musicBanks VALUES (?, ?, ?)", [assetId, assetName, files.length]);
        fse.writeSync(this._headerFile,`#define FW64_ASSET_musicbank_${assetName} ${assetId}\n`);

        return assetId;
    }

    addRaw(rawPath, size) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(rawPath, path.extname(rawPath));

        this._db.run("INSERT INTO rawFiles VALUES (?, ?, ?)", [assetId, rawPath, size]);
        fse.writeSync(this._headerFile,`#define FW64_ASSET_raw_${assetName} ${assetId}\n`);

        return assetId;
    }

    addLayerMap(layermap, index) {
        const assetId = this._nextAssetId++;

        this._db.run("INSERT INTO layerMaps VALUES (?, ?, ?)", [assetId, index, JSON.stringify(Object.fromEntries(layermap))]);

        return assetId;
    }

    addScene(name, index, layerMap, assetPath) {
        const assetId = this._nextAssetId++;
        const assetName = Util.safeDefineName(name);

        this._db.run("INSERT INTO scenes VALUES (?, ?, ?, ?)", [assetId, assetPath, index, layerMap]);
        fse.writeSync(this._headerFile,`#define FW64_ASSET_scene_${assetName} ${assetId}\n`);

        return assetId;
    }

    finalize() {
        this._db.close();

        const assetCount = this._nextAssetId - 1;
        fse.writeSync(this._headerFile, `#define FW64_ASSET_COUNT ${assetCount}\n\n`);
        fse.closeSync(this._headerFile);
    }
}

module.exports = Bundle;