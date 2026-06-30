const Desktop = require("../desktop/Process")
const Environment = require("../Environment");

/** The web platfrom makes use of the shared desktop pipeline. This function simply defines the correct environment and invokes the desktop pipeline*/
async function processWeb(manifestFile, assetDirectory, outputDirectory, pluginMap) {
    Desktop.runPipelineProcessor(manifestFile, assetDirectory, outputDirectory, pluginMap, "web", Environment.Architecture.Arch32);
}

module.exports = processWeb;