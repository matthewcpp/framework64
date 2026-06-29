const fs = require("fs");
const path = require("path");

const AnimationWriter = require("../animation/Writer");
const WriteInterface = require("../WriteInterface");

async function write(environment, mesh, animationData, meshFilePath, includeFilePath, meshWriter) {
    const file = fs.openSync(meshFilePath, "w");
    await meshWriter.writeStaticMeshToFile(environment, mesh, file);
    AnimationWriter.writeToFile(animationData, environment.binaryWriter, file);
    fs.closeSync(file);

    AnimationWriter.writeHeaderFile(animationData, mesh.name, includeFilePath);
}

function writeAnimationData(environment, animationData, meshName, dataFilePath, includeFilePath) {
    AnimationWriter.write(animationData, dataFilePath, environment.binaryWriter);
    AnimationWriter.writeHeaderFile(animationData, meshName, includeFilePath);
}

module.exports = {
    write: write,
    writeAnimationData: writeAnimationData
}
