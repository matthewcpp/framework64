const MaterialBundle = require("../gltf/MaterialBundle");
const GLTFLoader = require("../gltf/GLTFLoader");
const MeshWriter = require("./MeshWriter");

const path = require("path");

async function processMesh(meshJson, bundle, manifestDirectory, outputDirectory, plugins) {
    const srcPath = path.join(manifestDirectory, meshJson.src);
    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadFile(srcPath);

    if (!gltfLoader.gltf.meshes || gltfLoader.gltf.meshes.length === 0) {
        throw new Error(`glTF File: ${gltfPath} contains no meshes`);
    }

    const staticMesh = gltfLoader.meshes[0];
    const meshName = !!meshJson.name ? meshJson.name : path.basename(meshJson.src, path.extname(meshJson.src));
    staticMesh.name = meshName;

    staticMesh.materialBundle = new MaterialBundle(gltfLoader);
    staticMesh.materialBundle.bundleMeshMaterials(0);

    plugins.meshParsed(staticMesh, gltfLoader);
    
    const assetFileName = staticMesh.name + ".mesh";
    const destPath = path.join(outputDirectory, assetFileName);
    await MeshWriter.writeStaticMesh(staticMesh, destPath);
    
    bundle.addMesh(meshName, assetFileName);
}

module.exports = processMesh;