const GLMeshWriter = require("../GLMeshWriter");
const MaterialBundleWriter = require("./MaterialBundleWriter");
const WriteInterface = require("../WriteInterface");

const fs = require("fs");

/** Writes a self contained static mesh to file.
 *  Precondition: this gltf data should contain at least 1 mesh.
 */
async function writeStaticMesh(environment, staticMesh, destPath) {
    const file = fs.openSync(destPath, "w");
    await writeStaticMeshToFile(environment, staticMesh, file)
    fs.closeSync(file);
}

async function writeStaticMeshToFile(environment, staticMesh, file) {
    if (staticMesh.materialBundle === null) {
        throw new Error("Error writing static mesh: no material bundle present on mesh.");
    }

    await _writeMeshToFile(environment, staticMesh, staticMesh.materialBundle, file);
}

async function writeMeshData(environment, mesh, materialBundle, file) {
    if (mesh.materialBundle != null) {
        throw new Error("Error writing mesh data: unexpected material bundle present on mesh.");
    }
    await _writeMeshToFile(environment, mesh, materialBundle, file);
}

async function _writeMeshToFile(environment, mesh, materialBundle, file) {
    const writer = WriteInterface.littleEndian();

    GLMeshWriter.writeMeshInfo(mesh, writer, file)

    if (mesh.materialBundle) {
        const gltfData = mesh.materialBundle.gltfData;
        const images = await MaterialBundleWriter.createDesktopImages(gltfData);
        await MaterialBundleWriter.write(materialBundle, images, gltfData, file);
    }

    GLMeshWriter.writeMeshData(environment, mesh, materialBundle, writer, file);
}

module.exports = {
    writeStaticMesh: writeStaticMesh,
    writeStaticMeshToFile: writeStaticMeshToFile,
    writeMeshData: writeMeshData
};