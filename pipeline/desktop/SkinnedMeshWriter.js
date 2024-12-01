const fs = require("fs");
const path = require("path");

const MeshWriter = require("./MeshWriter");
const AnimationWriter = require("../animation/Writer");
const WriteInterface = require("../WriteInterface");

async function write(environment, mesh, animationData, meshFilePath, includeFilePath) {
    const file = fs.openSync(meshFilePath, "w");
    await MeshWriter.writeStaticMeshToFile(environment, mesh, file);
    AnimationWriter.writeToFile(animationData, WriteInterface.littleEndian(), file);
    fs.closeSync(file);

    AnimationWriter.writeHeaderFile(animationData, mesh.name, includeFilePath);
}

function writeAnimationData(animationData, meshName, dataFilePath, includeFilePath) {
    AnimationWriter.write(animationData, dataFilePath, WriteInterface.littleEndian());
    AnimationWriter.writeHeaderFile(animationData, meshName, includeFilePath);
}

module.exports = {
    write: write,
    writeAnimationData: writeAnimationData
}
