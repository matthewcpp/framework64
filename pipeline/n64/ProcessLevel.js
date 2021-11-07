const GLTFLoader = require("./GLTFLoader");
const SceneWriter = require("./SceneWriter")
const SceneDefines = require("../SceneDefines");

const path = require("path");

async function processLevel(level, typeMap, layerMap, archive, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, level.src);
    const gltfLoader = new GLTFLoader({});

    const scenes = await gltfLoader.loadLevel(sourceFile, new Map(Object.entries(typeMap)), new Map(Object.entries(layerMap)));

    for (const scene of scenes) {
        await SceneWriter.write(scene, outputDirectory, archive);

        const sceneIncludeFileName = `scene_${scene.name}.h`
        const sceneDefineFile = path.join(outputDirectory, "include", sceneIncludeFileName)
        SceneDefines.writeToFile(gltfLoader.gltf, scene.name, scene.rootNode, sceneDefineFile);
    }
}

module.exports = processLevel;