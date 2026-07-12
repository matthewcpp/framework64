const Environment = require("../Environment");
const N64LibUltraAssetBundle = require("./AssetBundle");

const BasicFileProcessor = require("../common/FileProcessor");
const GltfLevelProcessor = require("../common/LevelProcessor");
const GltfMeshProcessor = require("../common/MeshProcessor");
const GltfSkinnedMeshProcessor = require("../common/SkinnedMeshProcessor");
const FontProcessor = require("../common/FontProcessor");
const N64LibUltraFont = require("./Font");
const N64LibUltraFontWriter = require("./FontWriter");
const N64LibUltraImageProcessor = require("./ImageProcessor");
const N64LibUltraMaterialBundleWriter = require("./MaterialBundleWriter");
const N64LibUltraMeshWriter = require("./MeshWriter");
const N64LibUltraMusicBankProcessor = require("./MusicBankProcessor");
const N64LibUltraSoundBankProcessor = require("./SoundBankProcessor");
const PipelineProcessor = require("../common/PipelineProcessor");

const Util = require("../Util");
const fs = require("fs")
const path = require("path");


async function processN64(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const archive = new N64LibUltraAssetBundle(outputDirectory);
    const pipelinePath = path.normalize(path.join(__dirname, ".."));
    const environment = new Environment("n64_libultra", Environment.Architecture.Arch32, Environment.Endian.Big, archive, 
        manifestFile, assetDirectory, outputDirectory, includeDirectory, pipelinePath);

    const pipelineProcessor = new N64LibUltraPipelineProcessor(environment, pluginMap);
    await pipelineProcessor.process(manifest);

    archive.writeHeader(path.join(includeDirectory, "assets.h"));
    archive.writeArchive(path.join(outputDirectory, "assets.dat"));
    archive.writeManifest(path.join(outputDirectory, "manifest.txt"))
}

class N64LibUltraPipelineProcessor extends PipelineProcessor {
    constructor(environment, plugins) {
        super(environment);

        this._fileProcessor = new BasicFileProcessor(environment, plugins);
        this._musicBankProcessor = new N64LibUltraMusicBankProcessor(environment);
        this._soundBankProcessor = new N64LibUltraSoundBankProcessor(environment);

        this._imageProcessor = new N64LibUltraImageProcessor(environment);
        this._fontProcessor = new N64LibUltraFontProcessor(environment, this._imageProcessor);

        const materialBundleWriter = new N64LibUltraMaterialBundleWriter(this._imageProcessor);
        const meshWriter = new N64LibUltraMeshWriter(materialBundleWriter);

        this._levelProcessor = new GltfLevelProcessor(environment, materialBundleWriter, meshWriter);
        this._meshProcessor = new GltfMeshProcessor(environment, meshWriter);
        this._skinnedMeshProcessor = new GltfSkinnedMeshProcessor(environment, meshWriter);
    }
};

class N64LibUltraFontProcessor extends FontProcessor {
    constructor(environment, imageProcessor) {
        super(environment, imageProcessor, new N64LibUltraFontWriter());
    }

    _createFont(name) {
        return new N64LibUltraFont(name);
    }
};

module.exports = processN64;
