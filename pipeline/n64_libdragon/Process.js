const Environment = require("../Environment");

const BasicFileProcessor = require("../common/FileProcessor");
const FontProcessor = require("../common/FontProcessor");
const N64LibDragonImageProcessor = require("./ImageProcessor");
const N64LibDragonFont = require("./Font");
const N64LibDragonFontWriter = require("./FontWriter");
const N64LibDragonMusicBankProcessor = require("./MusicBankProcessor");
const N64LibDragonSoundBankProcessor = require("./SoundBankProcessor");

const DfsAssets = require("./DfsAssets");

const Util = require("../Util");

const fs = require("fs");
const fse = require("fs-extra");
const path = require("path");
const PipelineProcessor = require("../common/PipelineProcessor");

async function processN64Libdragon(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    console.log(`n64_libdragon build assets: ${outputDirectory}`);
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const dfsAssets = new DfsAssets();

    const pipelinePath = path.normalize(path.join(__dirname, ".."));
    const environment = new Environment("n64_libdragon", Environment.Architecture.Arch32, Environment.Endian.Big, dfsAssets, 
        manifestFile, assetDirectory, outputDirectory, includeDirectory, pipelinePath);

    const pipelineProcessor = new N64LibDragonPipelineProcessor(environment, pluginMap);
    await pipelineProcessor.process(manifest);

    // create the DFS asset bundle and write out the header and manifest
    const headerPath = path.join(Util.assetIncludeDirectory(outputDirectory), "assets.h");
    dfsAssets.writeHeader(headerPath);
    dfsAssets.writeManifest(path.join(outputDirectory, "dfs_manifest.txt"));

    // writes the combined asset bundle into the dfs directory, and then runs libdragon's mkdfs utility on it
    const dfsDirectory = path.join(outputDirectory, "dfs");
    fse.ensureDirSync(dfsDirectory);
    await dfsAssets.makeBundle(dfsDirectory, path.join(outputDirectory, "assets.dfs"));
}

class N64LibDragonPipelineProcessor extends PipelineProcessor {
    constructor(environment, plugins) {
        super(environment);

        this._fileProcessor = new BasicFileProcessor(environment, plugins);
        this._musicBankProcessor = new N64LibDragonMusicBankProcessor(environment);
        this._soundBankProcessor = new N64LibDragonSoundBankProcessor(environment);

        this._imageProcessor = new N64LibDragonImageProcessor(environment);
        this._fontProcessor = new N64LibDragonFontProcessor(environment, this._imageProcessor);
    }
}

class N64LibDragonFontProcessor extends FontProcessor {
    constructor(environment, imageProcessor) {
        super(environment, imageProcessor, new N64LibDragonFontWriter());
    }

    _createFont(name) {
        return new N64LibDragonFont(name);
    }
};

module.exports = processN64Libdragon;
