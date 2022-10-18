class Plugins {
    plugins = [];

    constructor(plugin) {
        if (!!plugin)
            this.plugins.push(plugin);
    }

    addPlugin(plugin) {
        this.plugins.push(plugin);
    }

    initialize(assetBundle, baseDirectory, outputDirectory, assetIncludeDirectory) {
        for (const plugin of this.plugins) {
            if (plugin.initialize)
                plugin.initialize(assetBundle, baseDirectory, outputDirectory, assetIncludeDirectory);
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
}

module.exports = Plugins;