const Environment = require("../Environment");

/** The desktop pipeline is shared with the web pipeline, this function simply creates the environment and calls the shared processing function. */
async function processDesktop(manifestFile, assetDirectory, outputDirectory, pluginMap, architecture) {
    require("./ProcessEnvironment")(manifestFile, assetDirectory, outputDirectory, pluginMap, "desktop", architecture);
}

module.exports = processDesktop;