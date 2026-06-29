const CollisionGeometryDebug = require("./CollisionGeometryDebug");
const LevelParser = require("../gltf/LevelParser");
const SceneDefines = require("./SceneDefines");
const Util = require("../Util");
const SceneWriter = require("./SceneWriter");

const path = require("path");

class GltfLevelProcessor {
    _environment;
    _materialBundleWriter;
    _meshWriter;

    constructor(environment, materialBundleWriter, meshWriter) {
        this._environment = environment;
        this._materialBundleWriter = materialBundleWriter;
        this._meshWriter = meshWriter;
    }

    async process(level, layerMap) {
        const srcPath = path.join(this._environment.assetDirectory, level.src);
        const levelParser = new LevelParser();
        await levelParser.parse(srcPath, layerMap);

        if (Object.hasOwn(level, "collisionGeometry") && level.collisionGeometry === true) {
            if (!Object.hasOwn(level, "grid")) {
                throw new Error("Must specify grid size (CountXxCountZ) in order to generate collision geometry.");
            }

            const dimensions = level.grid.split('x');

            levelParser.createCollisionGeometry(parseInt(dimensions[0]), parseInt(dimensions[1]));

            for (const scene of levelParser.scenes) {
                const collisionDebugFile = path.join(path.dirname(this._environment.outputDirectory), Util.safeDefineName(scene.name) +"_collision_info.txt");
                CollisionGeometryDebug.writeTextFile(scene.collisionGeometry, collisionDebugFile);
            }
        }

        for (const scene of levelParser.scenes) {
            await this._processScene(scene, levelParser);

            if (scene.collisionGeometry !== null) {
                const safeSceneName =  Util.safeDefineName(scene.name) + "_collision";
                const collisionDebugSceneFileName = safeSceneName + ".scene";
                const collisionDebugSceneFile = path.join(this._environment.outputDirectory, collisionDebugSceneFileName);

                await CollisionGeometryDebug.writeCollisionGeometryDebugData(this._environment, scene.collisionGeometry, this._environment.binaryWriter, collisionDebugSceneFile, this._materialBundleWriter, this._meshWriter);
                this._environment.assetBundle.addFile(collisionDebugSceneFile, safeSceneName);
            }
        }
    }

    async _processScene(scene, levelParser) {
        const safeSceneName =  Util.safeDefineName(scene.name);
        const sceneFileName = safeSceneName + ".scene";
        const sceneFile = path.join(this._environment.outputDirectory, sceneFileName);

        await SceneWriter.write(this._environment, scene, levelParser.gltfData, sceneFile, this._materialBundleWriter, this._meshWriter);
        this._environment.assetBundle.addScene(sceneFile, safeSceneName);

        const sceneIncludeFileName =`scene_${safeSceneName}.h`;
        const sceneDefineFile = path.join(this._environment.includeDirectory, sceneIncludeFileName)
        SceneDefines.writeToFile(scene, sceneDefineFile);
    }
};

module.exports = GltfLevelProcessor;
