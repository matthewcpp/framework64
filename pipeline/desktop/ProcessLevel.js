const GLTFLoader = require("../n64/GLTFLoader");

const fse = require("fs-extra");
const path = require("path");

const SceneDefines = require("../SceneDefines");
const Util = require("../Util");
const GLTFUtil = require("../GLTFUtil");

async function processLevel(level, layerMap, bundle, baseDirectory, outputDirectory, includeDirectory, plugins) {
    const srcPath = path.join(baseDirectory, level.src);

    // temporary until desktop asset rework
    const gltfDirName = path.dirname(level.src);
    const gltfSrcDir = path.join(baseDirectory, gltfDirName);
    const gltfDestDir = path.join(outputDirectory, gltfDirName);
    fse.copySync(gltfSrcDir, gltfDestDir);

    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadLevel(srcPath, layerMap);

    plugins.levelBegin(level, gltfLoader.gltf);

    const topLevelSceneNodeindices = GLTFUtil.extractTopLevelSceneNodeIndices(gltfLoader.gltf);

    for (const sceneNodeIndex of topLevelSceneNodeindices) {
        const sceneRootNode = gltfLoader.gltf.nodes[sceneNodeIndex];
        const sceneName = sceneRootNode.name.toLowerCase();
        const safeSceneName = Util.safeDefineName(sceneName);
        const sceneIncludeFileName = `scene_${safeSceneName}.h`

        bundle.addScene(sceneName, sceneNodeIndex, 0, level.src); // there is only a single layer map at index 0. todo: remove this?

        const sceneDefineFile = path.join(includeDirectory, sceneIncludeFileName)
        SceneDefines.writeToFile(gltfLoader.gltf, sceneName, GLTFUtil.getSceneRootNode(gltfLoader.gltf, sceneRootNode), sceneDefineFile);

        plugins.processScene(safeSceneName, sceneRootNode);
    }

    plugins.levelEnd();
}

module.exports = processLevel;