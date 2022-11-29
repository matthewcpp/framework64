const path = require("path");
const fse = require("fs-extra");

const GLTFLoader = require("../n64/GLTFLoader");

async function processMesh(mesh, bundle, manifestDirectory, outputDirectory, plugins) {
    const srcPath = path.join(manifestDirectory, mesh.src);
    const meshName = mesh.hasOwnProperty("name") ? mesh.name : path.basename(mesh.src, path.extname(mesh.src));

    // temporary until desktop asset rework
    const gltfDirName = path.dirname(mesh.src);
    const gltfSrcDir = path.join(manifestDirectory, gltfDirName);
    const gltfDestDir = path.join(outputDirectory, gltfDirName);
    fse.copySync(gltfSrcDir, gltfDestDir);

    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadStaticMesh(srcPath);
    plugins.meshParsed(mesh, gltfLoader);
    
    bundle.addMesh(meshName, mesh.src);
}

module.exports = processMesh;