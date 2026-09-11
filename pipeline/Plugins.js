const path = require("path");
const fse = require("fs-extra");

class Plugins {
    filePlugins;
    assetPlugins;

    _environment;

    _manifestDir;

    constructor(environment) {
        this._environment = environment;

        this.filePlugins = new Map();
        this.assetPlugins = [];
    }

    /// TODO: unify with plugin loading
    async initialize(environment){
        for (const plugin of this.assetPlugins) {
            if (typeof plugin.initialize === "function") {
                await plugin.initialize(environment);
            }
        }
    }

    async postProcessFont(fontJson) {
        for (const plugin of this.assetPlugins) {
            if (typeof plugin.postProcessFont === "function") {
                await plugin.postProcessFont(fontJson);
            }
        }
    }

    async postProcessSkinnedMesh(meshJson, meshData) {
        for (const plugin of this.assetPlugins) {
            if (typeof plugin.postProcessSkinnedMesh === "function") {
                await plugin.postProcessSkinnedMesh(meshJson, meshData);
            }
        }
    }

    async postProcessMesh(meshJson, meshData) {
        for (const plugin of this.assetPlugins) {
            if (typeof plugin.postProcessMesh === "function") {
                await plugin.postProcessMesh(meshJson, meshData);
            }
        }
    }

    _loadPlugin(plugInfo) {
        if (!Object.hasOwn(plugInfo, "src")) {
            throw new Error(`Plugin ${i} does not specify a 'src' attribute`);
        }

        const pluginPath = path.join(this._manifestDir, plugInfo.src);
        if (!fse.existsSync(pluginPath)) {
            throw new Error(`Plugin does not exist: ${pluginPath}`);
        }

        const pluginClass = require(pluginPath);

        if (typeof(pluginClass) !== "function") {
            throw new Error(`Plugin script should export a single class: ${plugInfo.src}`);
        }

        console.log(`Loaded plugin: ${plugInfo.src}`);
        return new pluginClass();
    }

    _loadFilePlugins(pluginManifest) {
        if (!Object.hasOwn(pluginManifest, "filePlugins")) {
            return;
        }

        for (let i = 0; i < pluginManifest.filePlugins.length; i++) {
            const plugInfo = pluginManifest.filePlugins[i];

            if (!Object.hasOwn(plugInfo, "extensions") || plugInfo.extensions.length === 0) {
                throw new Error(`Plugin does not declare any file extensions to process: ${plugInfo.src}`);
            }

            const pluginInstance = this._loadPlugin(plugInfo);

            for (const extension of plugInfo.extensions) {
                if (this.filePlugins.has(extension)) {
                    throw new Error(`Multiple plugins registered for extension: ${extension}`)
                }

                this.filePlugins.set(extension, pluginInstance);
            }
        }
    }

    _loadAssetPlugins(pluginManifest) {
        if (!Object.hasOwn(pluginManifest, "assetPlugins")) {
            return;
        }

        for (let i = 0; i < pluginManifest.assetPlugins.length; i++) {
            const plugInfo = pluginManifest.assetPlugins[i];
            const pluginInstance = this._loadPlugin(plugInfo);
            this.assetPlugins.push(pluginInstance);
        }
    }

    /** loads plugins given a manifest path.  This should be an absolute path. */
    load(pluginManifestPath) {
        this._manifestDir = path.dirname(pluginManifestPath);
        const pluginManifest = JSON.parse(fse.readFileSync(pluginManifestPath, {encoding: "utf8"}));

        this._loadFilePlugins(pluginManifest);
        this._loadAssetPlugins(pluginManifest);
    }
};

module.exports = Plugins;