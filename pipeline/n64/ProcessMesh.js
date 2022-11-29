const GLTFLoader = require("./GLTFLoader");
const MeshWriter = require("./N64MeshWriter")

const path = require("path");

async function processMesh(mesh, archive, baseDirectory, outputDirectory, plugins) {
    const sourceFile = path.join(baseDirectory, mesh.src);
    const gltfLoader = new GLTFLoader();
    const staticMesh = await gltfLoader.loadStaticMesh(sourceFile);

    plugins.meshParsed(mesh, gltfLoader);

    await MeshWriter.writeStaticMesh(staticMesh, outputDirectory, archive);
}

module.exports = processMesh;