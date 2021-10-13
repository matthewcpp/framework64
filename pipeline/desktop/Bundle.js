const sqlite3 = require('sqlite3').verbose();

const fs = require("fs");
const path = require("path");

class Bundle {
    _nextAssetId = 1
    _headerFile = 0;

    _db = null;
    _imageStmt = null;
    _fontStmt = null;
    _meshStmt = null;
    _soundBankStmt = null;
    _musicBankStmt = null;
    _rawFileStmt = null;
    _terrainStmt = null;
    _sceneStmt = null;

    constructor(outputDirectory) {
        this._initDatabase(outputDirectory);
        this._initHeaderFile(outputDirectory);
    }

    _initHeaderFile(outputDirectory) {
        const headerPath = path.join(outputDirectory, "assets.h");
        this._headerFile = fs.openSync(headerPath, "w");
    }

    _initDatabase(outputDirectory) {
        const dbPath = path.join(outputDirectory, "assets.db");
        const db = new sqlite3.Database(dbPath);

        db.serialize(() => {
            db.run("CREATE TABLE images (assetId INTEGER PRIMARY KEY, path TEXT, hslices INTEGER, vslices INTEGER)");
            db.run("CREATE TABLE fonts (assetId INTEGER PRIMARY KEY, path TEXT, size INTEGER, tileWidth INTEGER, tileHeight INTEGER, glyphCount INTEGER, glyphData BLOB )");
            db.run("CREATE TABLE meshes (assetId INTEGER PRIMARY KEY, path TEXT)");
            db.run("CREATE TABLE soundBanks (assetId INTEGER PRIMARY KEY, path TEXT, count INTEGER)");
            db.run("CREATE TABLE musicBanks (assetId INTEGER PRIMARY KEY, path TEXT, count INTEGER)");
            db.run("CREATE TABLE rawFiles (assetId INTEGER PRIMARY KEY, path TEXT, size INTEGER)");
            db.run("CREATE TABLE terrains (assetId INTEGER PRIMARY KEY, path TEXT, dimx INTEGER, dimz INTEGER)");
            db.run("CREATE TABLE scenes (assetId INTEGER PRIMARY KEY, path TEXT, typemap TEXT)");
        });

        this._imageStmt = db.prepare("INSERT INTO images VALUES (?, ?, ?, ?)");
        this._fontStmt = db.prepare("INSERT INTO fonts VALUES (?, ?, ?, ?, ?, ?, ?)");
        this._meshStmt = db.prepare("INSERT INTO meshes VALUES (?, ?)");
        this._soundBankStmt = db.prepare("INSERT INTO soundBanks VALUES (?, ?, ?)");
        this._musicBankStmt = db.prepare("INSERT INTO musicBanks VALUES (?, ?, ?)");
        this._rawFileStmt = db.prepare("INSERT INTO rawFiles VALUES (?, ?, ?)");
        this._terrainStmt = db.prepare("INSERT INTO terrains VALUES (?, ?, ?, ?)");
        this._sceneStmt = db.prepare("INSERT INTO scenes VALUES (?, ?, ?)");

        this._db = db;
    }

    addImage(imagePath, hslices, vslices) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(imagePath, path.extname(imagePath));

        this._imageStmt.run(assetId, imagePath, hslices, vslices);

        fs.writeSync(this._headerFile,`#define FW64_ASSET_image_${assetName} ${assetId}\n`);
    }

    addFont(font, fontPath) {
        const assetId = this._nextAssetId++;

        this._fontStmt.run(assetId, fontPath, font.size, font.tileWidth, font.tileHeight, font.glyphs.length, font.desktopGlyphBuffer);

        fs.writeSync(this._headerFile,`#define FW64_ASSET_font_${font.name} ${assetId}\n`);
    }

    addMesh(mesh, assetPath) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(mesh.src, path.extname(mesh.src));

        this._meshStmt.run(assetId, assetPath);

        fs.writeSync(this._headerFile,`#define FW64_ASSET_mesh_${assetName} ${assetId}\n`);
    }

    addSoundBank(soundBank, files) {
        const assetId = this._nextAssetId++;
        const assetName = soundBank.name;

        this._soundBankStmt.run(assetId, assetName, files.length);

        fs.writeSync(this._headerFile,`#define FW64_ASSET_soundbank_${assetName} ${assetId}\n`);

        for (let i = 0; i < files.length; i++) {
            const file = files[i];
            const soundName = path.basename(file, path.extname(file));
            fs.writeSync(this._headerFile,`#define FW64_ASSET_sound_${assetName}_${soundName}  ${i}\n`);
        }
    }

    addMusicBank(musicBank, files) {
        const assetId = this._nextAssetId++;
        const assetName = musicBank.name;

        this._musicBankStmt.run(assetId, assetName, files.length);

        fs.writeSync(this._headerFile,`#define FW64_ASSET_musicbank_${assetName} ${assetId}\n`);

        for (let i = 0; i < files.length; i++) {
            const file = files[i];
            const musicName = path.basename(file, path.extname(file));
            fs.writeSync(this._headerFile,`#define FW64_ASSET_music_${assetName}_${musicName}  ${i}\n`);
        }
    }

    addRaw(rawPath, size) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(rawPath, path.extname(rawPath));

        fs.writeSync(this._headerFile,`#define FW64_ASSET_raw_${assetName} ${assetId}\n`);

        this._rawFileStmt.run(assetId, rawPath, size);
    }

    addTerrain(terrain, assetPath) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(terrain.src, path.extname(terrain.src));

        fs.writeSync(this._headerFile,`#define FW64_ASSET_terrain_${assetName} ${assetId}\n`);

        this._terrainStmt.run(assetId, assetPath, terrain.dimensionX, terrain.dimensionZ);
    }

    addScene(scene, assetPath, typemapStr) {
        const assetId = this._nextAssetId++;
        const assetName = path.basename(scene.src, path.extname(scene.src));

        fs.writeSync(this._headerFile,`#define FW64_ASSET_scene_${assetName} ${assetId}\n`);

        this._sceneStmt.run(assetId, assetPath, typemapStr);
    }

    finalize() {
        this._imageStmt.finalize();
        this._fontStmt.finalize();
        this._meshStmt.finalize();
        this._soundBankStmt.finalize();
        this._musicBankStmt.finalize();
        this._rawFileStmt.finalize();
        this._terrainStmt.finalize();
        this._sceneStmt.finalize();
        this._db.close();

        const assetCount = this._nextAssetId - 1;
        fs.writeSync(this._headerFile, `#define FW64_ASSET_COUNT ${assetCount}\n\n`);
        fs.closeSync(this._headerFile);
    }
}

module.exports = Bundle;