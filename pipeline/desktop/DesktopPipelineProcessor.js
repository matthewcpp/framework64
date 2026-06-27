const BasicFileProcessor = require("../FileProcessor");
const DesktopFontProcessor = require("./FontProcessor");
const DesktopImageProcessor = require("./ImageProcessor");
const DesktopSoundBankProcessor = require("./SoundBankProcessor");
const DesktopMaterialBundleWriter = require("./MaterialBundleWriter");
const DesktopMeshWriter = require("./MeshWriter");
const DesktopMusicBankProcessor = require("./MusicBankProcessor");
const GltfLevelProcessor = require("../LevelProcessor");
const GltfMeshProcessor = require("../MeshProcessor");
const GltfSkinnedMeshProcessor = require("../SkinnedMeshProcessor");
const PipelineProcessor = require("../common/PipelineProcessor");

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

module.exports = DesktopPipelineProcessor;
