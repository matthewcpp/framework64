const GLTFLoader = require("../gltf/GLTFLoader");
const MaterialBundle = require("../gltf/MaterialBundle");
const MeshWriter = require("./MeshWriter");
const Util = require("../Util");

const path = require("path");

async function processMesh(meshJson, dfsAssets, manifestDirectory, outputDirectory, pluginMap) {
    const srcPath = path.join(manifestDirectory, meshJson.src);
    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadFile(srcPath);

    if (!gltfLoader.gltf.meshes || gltfLoader.gltf.meshes.length === 0) {
        throw new Error(`glTF File: ${gltfPath} contains no meshes`);
    }

    const staticMesh = gltfLoader.meshes[0];
    const meshName = Util.safeDefineName(!!meshJson.name ? meshJson.name : path.basename(meshJson.src, path.extname(meshJson.src)));
    staticMesh.name = meshName;

    staticMesh.materialBundle = new MaterialBundle(gltfLoader);
    staticMesh.materialBundle.bundleMeshMaterials(0);

    //TODO: This is the asset id for the mesh, images it contains will be written out separately
    // this will be fixed once asset_loadf_into is fixed
    const meshFileName = dfsAssets.addMesh(srcPath, meshName).toString();
    await MeshWriter.writeStaticMesh(staticMesh, dfsAssets, outputDirectory, meshFileName);
}

module.exports = processMesh;