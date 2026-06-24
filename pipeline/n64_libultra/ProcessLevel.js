const CollisionGeometryDebug = require("../CollisionGeometryDebug");
const LevelParser = require("../LevelParser");
const SceneWriter = require("./SceneWriter");
const SceneDefines = require("../SceneDefines");
const Util = require("../Util");

const path = require("path");

async function _processScene(environment, scene, levelParser, archive, outputDirectory, includeDirectory) {
    const safeSceneName =  Util.safeDefineName(scene.name);
    const sceneFileName = safeSceneName + ".scene";
    const sceneFile = path.join(outputDirectory, sceneFileName);

    await SceneWriter.write(environment, scene, levelParser.gltfData, sceneFile);
    archive.addScene(sceneFile, safeSceneName);

    const sceneIncludeFileName =`scene_${safeSceneName}.h`;
    const sceneDefineFile = path.join(includeDirectory, sceneIncludeFileName)
    SceneDefines.writeToFile(scene, sceneDefineFile);
}

async function processLevel(environment, level, layerMap, archive, baseDirectory, outputDirectory, includeDirectory) {
    const srcPath = path.join(baseDirectory, level.src);
    const levelParser = new LevelParser();
    await levelParser.parse(srcPath, layerMap);

    if (Object.hasOwn(level, "collisionGeometry") && level.collisionGeometry === true) {
        if (!Object.hasOwn(level, "grid")) {
            throw new Error("Must specify grid size (CountXxCountZ) in order to generate collision geometry.");
        }

        const dimensions = level.grid.split('x');

        levelParser.createCollisionGeometry(parseInt(dimensions[0]), parseInt(dimensions[1]));

        for (const scene of levelParser.scenes) {
            const collisionDebugFile = path.join(path.dirname(outputDirectory), Util.safeDefineName(scene.name) +"_collision_info.txt");
            CollisionGeometryDebug.writeTextFile(scene.collisionGeometry, collisionDebugFile);
        }
    }

    for (const scene of levelParser.scenes) {
        await _processScene(environment, scene, levelParser, archive, outputDirectory, includeDirectory);

        if (scene.collisionGeometry !== null) {
            const WriteInterface = require("../WriteInterface");

            const safeSceneName =  Util.safeDefineName(scene.name) + "_collision";
            const collisionDebugSceneFileName = safeSceneName + ".scene";
            const collisionDebugSceneFile = path.join(outputDirectory, collisionDebugSceneFileName);

            await CollisionGeometryDebug.writeCollisionGeometryDebugData(environment, scene.collisionGeometry,  WriteInterface.bigEndian(), SceneWriter, collisionDebugSceneFile);
            archive.addFile(collisionDebugSceneFile, safeSceneName);
        }
    }
}

module.exports = processLevel;