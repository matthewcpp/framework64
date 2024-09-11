const LevelParser = require("../LevelParser");
const SceneWriter = require("./SceneWriter");
const SceneDefines = require("../SceneDefines");
const Util = require("../Util");

const path = require("path");

async function processLevel(level, layerMap, bundle, baseDirectory, outputDirectory, includeDirectory) {
    const srcPath = path.join(baseDirectory, level.src);
    const levelParser = new LevelParser();
    await levelParser.parse(srcPath, layerMap);

    for (const scene of levelParser.scenes) {
        const safeSceneName =  Util.safeDefineName(scene.name);
        const sceneFileName = safeSceneName + ".scene";
        const sceneFile = path.join(outputDirectory, sceneFileName);
        
        await SceneWriter.write(scene, levelParser.gltfData, sceneFile);
        bundle.addScene(safeSceneName, sceneFileName);

        const sceneIncludeFileName =`scene_${safeSceneName}.h`;
        const sceneDefineFile = path.join(includeDirectory, sceneIncludeFileName)
        SceneDefines.writeToFile(scene, sceneDefineFile);
    }
}

module.exports = processLevel;