const {ConvertGltfToGLB} = require('gltf-import-export');

const fs = require("fs");
const path = require("path");

const Util = require("../Util");
const SceneDefines = require("../SceneDefines");

function writeSceneDefs(scene, srcPath, outputDirectory) {
    const sceneJson = JSON.parse(fs.readFileSync(srcPath, {encoding: "utf-8"}));
    const sceneName = path.basename(scene.src, path.extname(scene.src));
    const outputFilePath = path.join(Util.assetIncludeDirectory(outputDirectory), `scene_${sceneName}.h`);

    SceneDefines.writeToFile(sceneJson, sceneName, outputFilePath);
}

async function processScene(scene, typemap, layermap, bundle, baseDirectory, outputDirectory) {
    const destName = path.basename(scene.src, ".gltf") + ".glb";
    const srcPath = path.join(baseDirectory, scene.src);
    const destPath = path.join(outputDirectory, destName);
    ConvertGltfToGLB(srcPath, destPath);

    bundle.addScene(scene, destName);
    writeSceneDefs(scene, srcPath, outputDirectory);
}

module.exports = processScene;