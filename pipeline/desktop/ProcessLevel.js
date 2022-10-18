const {ConvertGltfToGLB} = require('gltf-import-export');

const fs = require("fs");
const path = require("path");

const SceneDefines = require("../SceneDefines");
const Util = require("../Util");
const GLTFUtil = require("../GLTFUtil");

async function processLevel(level, bundle, baseDirectory, outputDirectory, includeDirectory, plugins) {
    const destName = path.basename(level.src, ".gltf") + ".glb";
    const srcPath = path.join(baseDirectory, level.src);
    const destPath = path.join(outputDirectory, destName);
    ConvertGltfToGLB(srcPath, destPath);

    const gltf = JSON.parse(fs.readFileSync(srcPath, {encoding: "utf8"}));
    plugins.levelBegin(level, gltf);

    const topLevelSceneNodeindices = GLTFUtil.extractTopLevelSceneNodeIndices(gltf);

    for (const sceneNodeIndex of topLevelSceneNodeindices) {
        const sceneRootNode = gltf.nodes[sceneNodeIndex];
        const sceneName = sceneRootNode.name.toLowerCase();
        const safeSceneName = Util.safeDefineName(sceneName);
        const sceneIncludeFileName = `scene_${safeSceneName}.h`

        // todo index wont be needed when scene file type added for desktop
        bundle.addScene(sceneName, sceneNodeIndex, 0, destName); // there is only a single layer map at index 0. todo: remove this?

        const sceneDefineFile = path.join(includeDirectory, sceneIncludeFileName)
        SceneDefines.writeToFile(gltf, sceneName, GLTFUtil.getSceneRootNode(gltf, sceneRootNode), sceneDefineFile);

        plugins.processScene(safeSceneName, sceneRootNode);
    }

    plugins.levelEnd();
}

module.exports = processLevel;