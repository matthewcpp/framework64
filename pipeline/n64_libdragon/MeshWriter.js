const GLMeshWriter = require("../GLMeshWriter");
const MaterialBundleWrtier = require("./MaterialBundleWriter");
const WriteInterface = require("../WriteInterface");
const fs = require("fs");

async function writeStaticMesh(staticMesh, destPath) {
    const libdragonImages = await MaterialBundleWrtier.createLibdragonImages(staticMesh.materialBundle.gltfData);

    const writer = WriteInterface.bigEndian();
    const file = fs.openSync(destPath, "w");
    GLMeshWriter.writeMeshInfo(staticMesh, writer, file);
    MaterialBundleWrtier.write(staticMesh.materialBundle, libdragonImages, staticMesh.materialBundle.gltfData, file);
    GLMeshWriter.writeMeshData(staticMesh, staticMesh.materialBundle, writer, file);
    fs.closeSync(file);
}

module.exports = {
    writeStaticMesh: writeStaticMesh
}