const sqlite3 = require('sqlite3').verbose();

const fse = require("fs-extra");
const path = require("path");

const Util = require("../Util")

class Bundle {
    _nextAssetId = 1
    _headerFile = 0;

    _db = null;
    _animationDataStmt = null;
    _imageStmt = null;
    _fontStmt = null;
    _meshStmt = null;
    _soundBankStmt = null;
    _musicBankStmt = null;
    _rawFileStmt = null;
    _terrainStmt = null;
    _typemapStmt = null;
    _layermapStmt = null;
    _sceneStmt = null;

    constructor(outputDirectory) {
        this._initDatabase(outputDirectory);
        this._initHeaderFile(outputDirectory);
    }
    

    _initHeaderFile(outputDirectory) {
        const headerDir = path.join(outputDirectory, "include");
        const headerPath = path.join(headerDir, "assets.h");
        
        fse.ensureDirSync(headerDir);
        this._headerFile = fse.openSync(headerPath, "w");
    }

    _initDatabase(outputDirectory) {
        const dbPath = path.join(outputDirectory, "assets.db");
        const db = new sqlite3.Database(dbPath);

        db.serialize(() => {
            db.run("CREATE TABLE animationData (assetId INTEGER PRIMARY KEY, path TEXT)");
            db.run("CREATE TABLE images (assetId INTEGER PRIMARY KEY, path TEXT, hslices INTEGER, vslices INTEGER)");
            db.run("CREATE TABLE fonts (assetId INTEGER PRIMARY KEY, path TEXT, size INTEGER, tileWidth INTEGER, tileHeight INTEGER, glyphCount INTEGER, glyphData BLOB )");
            db.run("CREATE TABLE meshes (assetId INTEGER PRIMARY KEY, path TEXT, jointMap TEXT)");
            db.run("CREATE TABLE soundBanks (assetId INTEGER PRIMARY KEY, path TEXT, count INTEGER)");
            db.run("CREATE TABLE musicBanks (assetId INTEGER PRIMARY KEY, path TEXT, count INTEGER)");
            db.run("CREATE TABLE rawFiles (assetId INTEGER PRIMARY KEY, path TEXT, size INTEGER)");
            db.run("CREATE TABLE terrains (assetId INTEGER PRIMARY KEY, path TEXT, dimx INTEGER, dimz INTEGER)");
            db.run("CREATE TABLE typeMaps (assetId INTEGER PRIMARY KEY, jsonIndex INTEGER, jsonStr TEXT)");
            db.run("CREATE TABLE layerMaps (assetId INTEGER PRIMARY KEY, jsonIndex INTEGER, jsonStr TEXT)");
            db.run("CREATE TABLE scenes (assetId INTEGER PRIMARY KEY, path TEXT, sceneIndex INTEGER, typeMap INTEGER, layerMap INTEGER)");
        });

        this._animationDataStmt = db.prepare("INSERT INTO animationData VALUES (?, ?)");
        this._imageStmt = db.prepare("INSERT INTO images VALUES (?, ?, ?, ?)");
        this._fontStmt = db.prepare("INSERT INTO fonts VALUES (?, ?, ?, ?, ?, ?, ?)");
        this._meshStmt = db.prepare("INSERT INTO meshes VALUES (?, ?, ?)");
        this._soundBankStmt = db.prepare("INSERT INTO soundBanks VALUES (?, ?, ?)");
        this._musicBankStmt = db.prepare("INSERT INTO musicBanks VALUES (?, ?, ?)");
        this._rawFileStmt = db.prepare("INSERT INTO rawFiles VALUES (?, ?, ?)");
        this._terrainStmt = db.prepare("INSERT INTO terrains VALUES (?, ?, ?, ?)");
        this._typemapStmt = db.prepare("INSERT INTO typeMaps VALUES (?, ?, ?)");
        this._layermapStmt = db.prepare("INSERT INTO layerMaps VALUES (?, ?, ?)");
        this._sceneStmt = db.prepare("INSERT INTO scenes VALUES (?, ?, ?, ?, ?)");

        this._db = db;
    }

    addAnimationData(animationDataPath) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(animationDataPath, path.extname(animationDataPath));

        this._animationDataStmt.run(assetId, animationDataPath);
        this._rawFileStmt.run(assetId, animationDataPath, 0);

        fse.writeSync(this._headerFile,`#define FW64_ASSET_animation_data_${assetName} ${assetId}\n`);

        return assetId;
    }

    addImage(imagePath, hslices, vslices) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(imagePath, path.extname(imagePath));

        this._imageStmt.run(assetId, imagePath, hslices, vslices);

        fse.writeSync(this._headerFile,`#define FW64_ASSET_image_${assetName} ${assetId}\n`);

        return assetId;
    }

    addFont(font, fontPath) {
        const assetId = this._nextAssetId++;

        this._fontStmt.run(assetId, fontPath, font.size, font.tileWidth, font.tileHeight, font.glyphs.length, font.desktopGlyphBuffer);

        fse.writeSync(this._headerFile,`#define FW64_ASSET_font_${font.name} ${assetId}\n`);

        return assetId;
    }

    addMesh(mesh, assetPath, jointMap) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(mesh.src, path.extname(mesh.src));

        this._meshStmt.run(assetId, assetPath, jointMap ? JSON.stringify(jointMap): null);

        fse.writeSync(this._headerFile,`#define FW64_ASSET_mesh_${assetName} ${assetId}\n`);

        return assetId;
    }

    addSoundBank(soundBank, files) {
        const assetId = this._nextAssetId++;
        const assetName = soundBank.name;

        this._soundBankStmt.run(assetId, assetName, files.length);

        fse.writeSync(this._headerFile,`#define FW64_ASSET_soundbank_${assetName} ${assetId}\n`);

        return assetId;
    }

    addMusicBank(musicBank, files) {
        const assetId = this._nextAssetId++;
        const assetName = musicBank.name;

        this._musicBankStmt.run(assetId, assetName, files.length);

        fse.writeSync(this._headerFile,`#define FW64_ASSET_musicbank_${assetName} ${assetId}\n`);

        return assetId;
    }

    addRaw(rawPath, size) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(rawPath, path.extname(rawPath));

        fse.writeSync(this._headerFile,`#define FW64_ASSET_raw_${assetName} ${assetId}\n`);

        this._rawFileStmt.run(assetId, rawPath, size);

        return assetId;
    }

    addTypeMap(typemap, index) {
        const assetId = this._nextAssetId++;
        this._typemapStmt.run(assetId, index, JSON.stringify(typemap));

        return assetId;
    }

    addLayerMap(layermap, index) {
        const assetId = this._nextAssetId++;
        this._layermapStmt.run(assetId, index, JSON.stringify(layermap));

        return assetId;
    }

    addScene(name, index, typeMap, layerMap, assetPath) {
        const assetId = this._nextAssetId++;
        const assetName = Util.safeDefineName(name);

        fse.writeSync(this._headerFile,`#define FW64_ASSET_scene_${assetName} ${assetId}\n`);

        this._sceneStmt.run(assetId, assetPath, index, typeMap, layerMap);

        return assetId;
    }

    finalize() {
        this._animationDataStmt.finalize();
        this._imageStmt.finalize();
        this._fontStmt.finalize();
        this._meshStmt.finalize();
        this._soundBankStmt.finalize();
        this._musicBankStmt.finalize();
        this._rawFileStmt.finalize();
        this._terrainStmt.finalize();
        this._typemapStmt.finalize();
        this._layermapStmt.finalize();
        this._sceneStmt.finalize();
        this._db.close();

        const assetCount = this._nextAssetId - 1;
        fse.writeSync(this._headerFile, `#define FW64_ASSET_COUNT ${assetCount}\n\n`);
        fse.closeSync(this._headerFile);
    }
}

module.exports = Bundle;