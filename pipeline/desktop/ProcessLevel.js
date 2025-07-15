const CollisionGeometryDebug = require("../CollisionGeometryDebug");
const LevelParser = require("../LevelParser");
const SceneWriter = require("./SceneWriter");
const SceneDefines = require("../SceneDefines");
const Util = require("../Util");

const path = require("path");

async function _processScene(environment, scene, levelParser, bundle, outputDirectory, includeDirectory) {
    const safeSceneName =  Util.safeDefineName(scene.name);
    const sceneFileName = safeSceneName + ".scene";
    const sceneFile = path.join(outputDirectory, sceneFileName);
    
    await SceneWriter.write(environment, scene, levelParser.gltfData, sceneFile);
    bundle.addScene(sceneFileName, safeSceneName);

    const sceneIncludeFileName =`scene_${safeSceneName}.h`;
    const sceneDefineFile = path.join(includeDirectory, sceneIncludeFileName)
    SceneDefines.writeToFile(scene, sceneDefineFile);
}

async function processLevel(environment, level, layerMap, bundle, baseDirectory, outputDirectory, includeDirectory) {
    const srcPath = path.join(baseDirectory, level.src);
    const levelParser = new LevelParser();
    await levelParser.parse(srcPath, layerMap);

    if (Object.hasOwn(level, "collisionGeometry") && level.collisionGeometry === true) {
        levelParser.createCollisionGeometry();

        for (const scene of levelParser.scenes) {
            const collisionDebugFile = path.join(path.dirname(outputDirectory), Util.safeDefineName(scene.name) +"_collision_info.txt");
            CollisionGeometryDebug.writeTextFile(scene.collisionGeometry, collisionDebugFile);
        }
    }

    for (const scene of levelParser.scenes) {
        await _processScene(environment, scene, levelParser, bundle, outputDirectory, includeDirectory);
        
        if (scene.collisionGeometry !== null) {
            const collisionGeometrySceneData = CollisionGeometryDebug.createWireScene(scene.collisionGeometry, Util.safeDefineName(scene.name));
            await _processScene(environment, collisionGeometrySceneData.scenes[0], collisionGeometrySceneData, bundle, outputDirectory, includeDirectory);
        }
    }
}

module.exports = processLevel;