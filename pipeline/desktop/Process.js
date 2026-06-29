const BasicFileProcessor = require("../common/FileProcessor");
const DesktopAssetBundle = require("./AssetBundle");
const DesktopFontProcessor = require("./FontProcessor");
const DesktopImageProcessor = require("./ImageProcessor");
const DesktopSoundBankProcessor = require("./SoundBankProcessor");
const DesktopMaterialBundleWriter = require("./MaterialBundleWriter");
const DesktopMeshWriter = require("./MeshWriter");
const DesktopMusicBankProcessor = require("./MusicBankProcessor");
const Environment = require("../Environment");
const GltfLevelProcessor = require("../common/LevelProcessor");
const GltfMeshProcessor = require("../common/MeshProcessor");
const GltfSkinnedMeshProcessor = require("../common/SkinnedMeshProcessor");
const PipelineProcessor = require("../common/PipelineProcessor");
const Util = require("../Util");

const fs = require("fs")
const path = require("path");

/** The desktop pipeline is shared with the web pipeline, this function simply creates the environment and calls the shared processing function. */
async function processDesktop(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    runPipelineProcessor(manifestFile, assetDirectory, outputDirectory, pluginMap, "desktop", Environment.Architecture.Arch64);
}

async function runPipelineProcessor(manifestFile, assetDirectory, outputDirectory, pluginMap, platform, arch) {
    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));
    const includeDirectory = Util.assetIncludeDirectory(outputDirectory);
    const bundle = new DesktopAssetBundle(outputDirectory);

    const pipelinePath = path.normalize(path.join(__dirname, ".."));
    const environment = new Environment(platform, arch, Environment.Endian.Little, 
        bundle, manifestFile, assetDirectory, outputDirectory, includeDirectory, pipelinePath);

    const desktopPipelineProcessor = new DesktopPipelineProcessor(environment, pluginMap);
    await desktopPipelineProcessor.process(manifest);

    bundle.writeHeader(path.join(includeDirectory, "assets.h"));
    bundle.writeAssetBundle(path.join(outputDirectory, "asset_bundle.txt"));
    bundle.writeManifest(path.join(outputDirectory, "manifest.txt"))
}

class DesktopPipelineProcessor extends PipelineProcessor {
    constructor(environment, plugins) {
        super(environment);

        this._fileProcessor = new BasicFileProcessor(environment, plugins);
        this._musicBankProcessor = new DesktopMusicBankProcessor(environment);
        this._soundBankProcessor = new DesktopSoundBankProcessor(environment);

        this._imageProcessor = new DesktopImageProcessor(environment);
        this._fontProcessor = new DesktopFontProcessor(environment, this._imageProcessor);

        const materialBundleWriter = new DesktopMaterialBundleWriter(environment, this._imageProcessor);
        const meshWriter = new DesktopMeshWriter(materialBundleWriter);
    
        this._levelProcessor = new GltfLevelProcessor(environment, materialBundleWriter, meshWriter);
        this._meshProcessor = new GltfMeshProcessor(environment, meshWriter);
        this._skinnedMeshProcessor = new GltfSkinnedMeshProcessor(environment, meshWriter);
    }
};

module.exports = {
    processDesktop: processDesktop,
    runPipelineProcessor: runPipelineProcessor
};