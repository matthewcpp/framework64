const {ConvertGltfToGLB} = require('gltf-import-export');
const path = require("path");

async function processTerrain(terrain, bundle, baseDirectory, outputDirectory) {
    const destName = path.basename(terrain.src, ".gltf") + ".glb";
    const srcPath = path.join(baseDirectory, terrain.src);
    const destPath = path.join(outputDirectory, destName);
    ConvertGltfToGLB(srcPath, destPath);

    bundle.addTerrain(terrain, destName);
}

module.exports = processTerrain;