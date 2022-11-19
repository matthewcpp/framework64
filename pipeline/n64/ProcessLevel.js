const GLTFLoader = require("./GLTFLoader");
const SceneWriter = require("./SceneWriter")
const SceneDefines = require("../SceneDefines");
const Util = require("../Util");
const GLTFUtil = require("../GLTFUtil");

const path = require("path");

async function processLevel(level, layerMap, archive, baseDirectory, outputDirectory, includeDirectory, plugins) {
    const sourceFile = path.join(baseDirectory, level.src);
    const gltfLoader = new GLTFLoader();

    const scenes = await gltfLoader.loadLevel(sourceFile, layerMap);
    plugins.levelBegin(level, gltfLoader.gltf)

    for (const scene of scenes) {
        await SceneWriter.write(scene, outputDirectory, archive);

        const safeSceneName =  Util.safeDefineName(scene.name);
        const sceneIncludeFileName =`scene_${safeSceneName}.h`;
        const sceneDefineFile = path.join(includeDirectory, sceneIncludeFileName)
        SceneDefines.writeToFile(gltfLoader.gltf, scene.name,  GLTFUtil.getSceneRootNode(gltfLoader.gltf, scene.rootNode), sceneDefineFile);
        plugins.processScene(scene.name, scene.rootNode);
    }

    plugins.levelEnd();
}

module.exports = processLevel;