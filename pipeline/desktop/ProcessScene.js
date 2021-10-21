const {ConvertGltfToGLB} = require('gltf-import-export');
const path = require("path");

async function processScene(scene, typemap, layermap, bundle, baseDirectory, outputDirectory) {
    const destName = path.basename(scene.src, ".gltf") + ".glb";
    const srcPath = path.join(baseDirectory, scene.src);
    const destPath = path.join(outputDirectory, destName);
    ConvertGltfToGLB(srcPath, destPath);

    bundle.addScene(scene, destName);
}

module.exports = processScene;