class ExampleAssetPlugin {
    async initialize (environment) {
        console.log(`ExampleAssetPlugin.postProcessFont: ${environment.platform}`);
    }

    async postProcessFont(fontJson) {
        console.log(`ExampleAssetPlugin.postProcessFont: ${fontJson.src}`);
    }
}

module.exports = ExampleAssetPlugin;