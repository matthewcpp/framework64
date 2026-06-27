const GLMeshWriter = require("../GLMeshWriter");
const MaterialBundleWriter = require("./MaterialBundleWriter");
const WriteInterface = require("../WriteInterface");

const fs = require("fs");

class DesktopMeshWriter {
    _materialBundleWriter;

    constructor(materialBundleWriter) {
        this._materialBundleWriter = materialBundleWriter;
    }

    /** Writes a self contained static mesh to file.
     *  Precondition: this gltf data should contain at least 1 mesh.
     */
    async writeStaticMesh(environment, staticMesh, destPath) {
        const file = fs.openSync(destPath, "w");
        await this.writeStaticMeshToFile(environment, staticMesh, file)
        fs.closeSync(file);
    }

    async writeStaticMeshToFile(environment, staticMesh, file) {
        if (staticMesh.materialBundle === null) {
            throw new Error("Error writing static mesh: no material bundle present on mesh.");
        }

        await this._writeMeshToFile(environment, staticMesh, staticMesh.materialBundle, file);
    }

    async writeMeshData(environment, mesh, materialBundle, images, file) {
        if (mesh.materialBundle != null) {
            throw new Error("Error writing mesh data: unexpected material bundle present on mesh.");
        }
        await this._writeMeshToFile(environment, mesh, materialBundle, file);
    }

    async _writeMeshToFile(environment, mesh, materialBundle, file) {
        const writer = WriteInterface.littleEndian();

        GLMeshWriter.writeMeshInfo(mesh, writer, file)

        if (mesh.materialBundle) {
            const gltfData = mesh.materialBundle.gltfData;
            const images = await this._materialBundleWriter.createImages(gltfData);
            await this._materialBundleWriter.write(materialBundle, images, gltfData, file);
        }

        GLMeshWriter.writeMeshData(environment, mesh, materialBundle, writer, file);
    }
};

module.exports = DesktopMeshWriter;
