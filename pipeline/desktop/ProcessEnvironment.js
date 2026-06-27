const DesktopAssetBundle = require("./AssetBundle");
const Environment = require("../Environment");
const Util = require("../Util");

const DesktopPipelineProcessor = require("./DesktopPipelineProcessor");

const fs = require("fs")
const path = require("path");

async function processDesktopEnvironment(manifestFile, assetDirectory, outputDirectory, pluginMap, platform, arch) {

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

module.exports = processDesktopEnvironment;