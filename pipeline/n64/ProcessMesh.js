const GLTFLoader = require("./GLTFLoader");
const MeshWriter = require("./N64MeshWriter")

const N64Material = require("./N64Material");

const path = require("path");

async function processMesh(mesh, archive, baseDirectory, outputDirectory) {
    const meshOptions = {};

    const sourceFile = path.join(baseDirectory, mesh.src);
    const gltfLoader = new GLTFLoader(meshOptions);
    const staticMesh = await gltfLoader.loadStaticMesh(sourceFile);

    await MeshWriter.writeStaticMesh(staticMesh, outputDirectory, archive);
}

module.exports = processMesh;