const path = require("path");
const util = require("util");

const execFile = util.promisify(require('child_process').execFile);

/* Determines if the current pipeline needs to run in docker, and if it does, launches the container and runs the pipeline.
   Note that the return value of the function does not indicate success of the pipeline, only if a container was launched.
*/
async function runInDocker(manifestFile, assetDirectory, platform, outputDirectory, pluginManifest) {
    const currentlyRunningInDocker = Object.hasOwn(process.env, "FW64_DOCKER_CONTAINER");

    // We are currently running in the correct container, nothing to do here, continue on with the asset pipeline processing
    if (currentlyRunningInDocker && platform === process.env.FW64_DOCKER_CONTAINER) {
        return false;
    }

    if (currentlyRunningInDocker && platform !== process.env.FW64_DOCKER_CONTAINER) {
        throw new Error(`Attempting to build pipeline for ${platform} while running in ${process.env.FW64_DOCKER_CONTAINER} container.`)
    }

    // at this point we need to launch the correct container
    switch (platform) {
        case "desktop":
        case "n64_libultra": // TODO: this should eventually be run in the container as well.
            return false;
        
        case "n64_libdragon":
            await runPipelineInContainer(manifestFile, assetDirectory, platform, outputDirectory, pluginManifest);
            return true;

        default:
            throw new Error(`Error launching docker container for platform: ${platform}`);
    }
}

async function runPipelineInContainer(assetManifestFile, assetDirectory, platform, outputDirectory, pluginManifestFile) {
    const containerName = `matthewcpp/framework64_${platform}`;
    console.log(`Performing pipeline build in container: ${containerName}`);

    const pipelineDir = path.dirname(__filename);
    const assetManifestDir = path.dirname(assetManifestFile);

    // note we specifically use posix here to get the '/' separator so that path will be correct if performing build on windows.
    const containerAssetManifestFile = path.posix.join("/manifest_dir", path.basename(assetManifestFile));

    const bindMounts = [
        "-v", `${pipelineDir}:/opt/framework64/pipeline`,
        "-v", `${assetManifestDir}:/manifest_dir`,
        "-v", `${assetDirectory}:/asset_dir`,
        "-v", `${outputDirectory}:/output_dir`,
    ];

    const pipelineArgs = [
        platform, 
        containerAssetManifestFile, 
        "/asset_dir", 
        "/output_dir"
    ];

    if (pluginManifestFile !== null) {
        const pluginDir = path.dirname(pluginManifest);
        const containerManifestFile = path.join("/plugin_dir", path.basename(assetManifestFile));

        bindMounts.push("-v", `${pluginDir}:/plugin_dir`);
        pipelineArgs.push(containerManifestFile)
    }

    const runPipelineArgs = [
        "run", "--rm",
        "--platform", "linux/amd64",
        ...bindMounts,
        containerName,
        "node", "/opt/framework64/pipeline/RunPipeline.js",
        ...pipelineArgs
    ];

    // console.log(runPipelineArgs.join(' '));

    const result = await execFile("docker", runPipelineArgs);
    console.log(result.stdout);
    console.log(result.stderr);
}

module.exports = runInDocker;