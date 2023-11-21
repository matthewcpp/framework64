const fs = require("fs");
const path = require("path");

const MeshWriter = require("./MeshWriter");
const AnimationWriter = require("../animation/Writer");
const WriteInterface = require("../WriteInterface");

async function write(mesh, animationData, meshFilePath, includeFilePath) {
    const writer = WriteInterface.bigEndian();

    const file = fs.openSync(meshFilePath, "w");
    await MeshWriter.writeStaticMeshToFile(mesh, file);
    AnimationWriter.writeToFile(animationData, writer, file);
    fs.closeSync(file);

    AnimationWriter.writeHeaderFile(animationData, mesh.name, includeFilePath);
}

function writeAnimationData(animationData, meshName, dataFilePath, includeFilePath) {
    const writer = WriteInterface.bigEndian();
    AnimationWriter.write(animationData, dataFilePath, writer);
    AnimationWriter.writeHeaderFile(animationData, meshName, includeFilePath);
}

module.exports = {
    write: write,
    writeAnimationData: writeAnimationData
}
