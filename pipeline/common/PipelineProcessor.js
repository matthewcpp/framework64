const BasicFileProcessor = require("./FileProcessor");
const LayersProcessor = require("./LayersProcessor");

const path = require("path");

/** Base class for asset manifest processing.
 * Handles asset type iteration and checking for required fields on each asset.
 */
class PipelineProcessor {
    _environment;
    _fileProcessor;
    _fontProcessor;
    _imageProcessor;
    _levelProcessor;
    _meshProcessor;
    _musicBankProcessor;
    _skinnedMeshProcessor;
    _soundBankProcessor;
    _plugins;

    constructor(_environment, plugins) {
        this._environment = _environment;
        this._plugins = plugins;
    }

    async process(manifest) {
        // Note: the layers processor will gracefully handle the case where there are no layers specified
        const potentialLayersPath = path.join(path.dirname(this._environment.manifestFile), "layers.json");
        const layers = new LayersProcessor(this._environment).process(potentialLayersPath);

        if (manifest.images) {
            for (const image of manifest.images) {
                if (image.src) {
                    console.log(`Processing Image: ${image.src}`);
                }
                else if (image.frames || image.frameDir){
                    console.log(`Processing Image Atlas: ${image.name}`);
                }

                await this._imageProcessor.process(image);
            }
        }

        if (manifest.fonts) {
            for (const font of manifest.fonts) {
                if (font.src) {
                    console.log(`Processing Font: ${font.src}`);
                }
                else{
                    console.log(`Processing Image Font: ${font.name}`);
                }
                
                await this._fontProcessor.process(font);
                await this._plugins.postProcessFont(font);
            }
        }

        if (manifest.meshes) {
            for (const mesh of manifest.meshes) {
                this._checkRequiredFields("mesh", mesh, ["src"]);
                console.log(`Processing Mesh: ${mesh.src}`)
                const meshData = await this._meshProcessor.process(mesh);
                await this._plugins.postProcessMesh(mesh, meshData);
            }
        }

        if (manifest.skinnedMeshes) {
            for (const skinnedMesh of manifest.skinnedMeshes) {
                this._checkRequiredFields("skinnedMesh", skinnedMesh, ["src"]);
                console.log(`Processing Skinned Mesh: ${skinnedMesh.src}`);
                const meshData = await this._skinnedMeshProcessor.process(skinnedMesh);
                await this._plugins.postProcessSkinnedMesh(skinnedMesh, meshData);
                
            }
        }

        if (manifest.levels) {
            for (const level of manifest.levels) {
                this._checkRequiredFields("level", level, ["src"]);
                console.log(`Processing Level: ${level.src}`);

                await this._levelProcessor.process(level, layers);
            }
        }

        if (manifest.soundBanks) {
            for (const soundBank of manifest.soundBanks) {
                this._checkRequiredFields("soundBank", soundBank, ["name", "dir"]);
                console.log(`Processing Sound Bank: ${soundBank.dir}`);

                await this._soundBankProcessor.process(soundBank);
            }
        }

        if (manifest.musicBanks) {
            for (const musicBank of manifest.musicBanks) {
                this._checkRequiredFields("musicBank", musicBank, ["name", "dir"]);
                console.log(`Processing Music Bank: ${musicBank.dir}`);

                await this._musicBankProcessor.process(musicBank);
            }
        }

        if (manifest.files) {
            for (const file of manifest.files) {
                this._checkRequiredFields("file", file, ["src"]);
                console.log(`Processing File: ${file.src}`);
                await this._fileProcessor.process(file);
            }
        }
    }

    _checkRequiredFields(type, obj, fields) {
        for (const field of fields) {
            if (!obj.hasOwnProperty(field))
                throw new Error(`${type} object must have the following properties: `+ fields.join(' '));
    }
}
};

module.exports = PipelineProcessor;