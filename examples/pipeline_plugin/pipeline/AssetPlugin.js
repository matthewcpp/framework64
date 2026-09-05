class ExampleAssetPlugin {
    async postProcessFont(fontJson) {
        console.log(`ExampleAssetPlugin.postProcessFont: ${fontJson.src}`);
    }
}

module.exports = ExampleAssetPlugin;