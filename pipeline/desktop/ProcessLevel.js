const {ConvertGltfToGLB} = require('gltf-import-export');

const fs = require("fs");
const path = require("path");

const SceneDefines = require("../SceneDefines");
const Util = require("../Util");

function extractScenes(gltf) {
    const sceneNode = gltf.scenes[gltf.scene];

    const scenes = [];

    for (const rootNodeIndex of sceneNode.nodes) {
        const rootNode = gltf.nodes[rootNodeIndex];
        if (getSceneNode(gltf, rootNode))
            scenes.push(rootNodeIndex);
    }

    return scenes;
}

function getSceneNode(gltf, rootNode) {
    // a node is a scene root node if it has a child that begins with Scene
    for (const childIndex of rootNode.children) {
        const childNode = gltf.nodes[childIndex];

        if (!childNode.name)
            continue;

        if (childNode.name.startsWith("Scene")) {
            return childNode;
        }
    }

    return null;
}

async function processLevel(level, typemap, layermap, bundle, baseDirectory, outputDirectory) {
    const destName = path.basename(level.src, ".gltf") + ".glb";
    const srcPath = path.join(baseDirectory, level.src);
    const destPath = path.join(outputDirectory, destName);
    ConvertGltfToGLB(srcPath, destPath);

    const gltf = JSON.parse(fs.readFileSync(srcPath, {encoding: "utf8"}));

    const sceneNodeIndices = extractScenes(gltf);

    for (const sceneNodeIndex of sceneNodeIndices) {
        const rootNode = gltf.nodes[sceneNodeIndex];

        const sceneName = rootNode.name.toLowerCase();
        bundle.addScene(sceneName, sceneNodeIndex, level.typeMap, level.layerMap, destName);

        const sceneIncludeFileName = `scene_${Util.safeDefineName(sceneName)}.h`
        const sceneDefineFile = path.join(outputDirectory, "include", sceneIncludeFileName)
        SceneDefines.writeToFile(gltf, sceneName, getSceneNode(gltf, rootNode), sceneDefineFile);
    }
}

module.exports = processLevel;