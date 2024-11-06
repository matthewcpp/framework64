const GLTFLoader = require("../gltf/GLTFLoader");
const MaterialBundle = require("../gltf/MaterialBundle");
const MeshWriter = require("./MeshWriter");

const path = require("path");

async function processMesh(meshJson, archive, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, meshJson.src);
    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadFile(sourceFile);

    if (!gltfLoader.gltf.meshes || gltfLoader.gltf.meshes.length === 0) {
        throw new Error(`glTF File: ${gltfPath} contains no meshes`);
    }

    const staticMesh = gltfLoader.meshes[0];
    const meshName = !!meshJson.name ? meshJson.name : path.basename(meshJson.src, path.extname(meshJson.src));
    staticMesh.name = meshName;

    staticMesh.materialBundle = new MaterialBundle(gltfLoader);
    staticMesh.materialBundle.bundleMeshMaterials(0);

    const destPath = path.join(outputDirectory, `${staticMesh.name}.mesh`);
    await MeshWriter.writeStaticMesh(staticMesh, destPath);

    archive.addMesh(destPath, staticMesh.name);
}

module.exports = processMesh;