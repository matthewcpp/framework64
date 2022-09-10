const path = require("path");
const fse = require("fs-extra");

const {ConvertGltfToGLB} = require('gltf-import-export');

// if the source file is a GLB file already, simply copy it into place
function processGlbFile(mesh, bundle, manifestDirectory, outputDirectory) {
    const srcPath = path.join(manifestDirectory, mesh.src);
    const destPath = path.join(outputDirectory, mesh.src);
    fse.copySync(srcPath, destPath);
    bundle.addMesh(mesh, mesh.src);
}

// gltf file should be converted to GLB for export
function processGltfFile(mesh, bundle, manifestDirectory, outputDirectory) {
    const srcPath = path.join(manifestDirectory, mesh.src);
    const meshName = mesh.hasOwnProperty("name") ? mesh.name : path.basename(mesh.src, path.extname(mesh.src));
    const destName = meshName + ".glb";
    
    const destPath = path.join(outputDirectory, destName);
    ConvertGltfToGLB(srcPath, destPath);
    bundle.addMesh(meshName, destName);
}

async function processMesh(mesh, bundle, manifestDirectory, outputDirectory) {
    if (path.extname(mesh.src) === ".glb") {
        processGlbFile(mesh, bundle, manifestDirectory, outputDirectory);
    }
    else {
        processGltfFile(mesh, bundle, manifestDirectory, outputDirectory);
    }
}

module.exports = processMesh;