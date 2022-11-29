class Plugins {
    plugins = [];

    constructor(plugins) {
        if (!!plugins)
            this.plugins.push(...plugins);
    }

    addPlugin(plugin) {
        this.plugins.push(plugin);
    }

    initialize(assetBundle, baseDirectory, outputDirectory, assetIncludeDirectory, platform) {
        for (const plugin of this.plugins) {
            if (plugin.initialize)
                plugin.initialize(assetBundle, baseDirectory, outputDirectory, assetIncludeDirectory, platform);
        }
    }

    levelBegin(level, gltf) {
        for (const plugin of this.plugins) {
            if (plugin.levelBegin)
                plugin.levelBegin(level, gltf);
        }
    }

    levelEnd() {
        for (const plugin of this.plugins) {
            if (plugin.levelEnd)
                plugin.levelEnd();
        }
    }

    processScene(name, gltfSceneNode) {
        for (const plugin of this.plugins) {
            if (plugin.processScene)
                plugin.processScene(name, gltfSceneNode);
        }
    }

    meshParsed(meshJson, gltfLoader) {
        for (const plugin of this.plugins) {
            if (plugin.meshParsed)
                plugin.meshParsed(meshJson, gltfLoader);
        }
    }

    skinnedMeshParsed(meshJson, gltfLoader, animationData) {
        for (const plugin of this.plugins) {
            if (plugin.skinnedMeshParsed)
                plugin.skinnedMeshParsed(meshJson, gltfLoader);
        }
    }
}

module.exports = Plugins;