const GLTFLoader = require("./GLTFLoader");
const SceneWriter = require("./SceneWriter")

const path = require("path");
const SceneDefines = require("../SceneDefines");

function writeSceneDefs(scene, gltf, outputDirectory) {
    const sceneName = path.basename(scene.src, path.extname(scene.src));
    const outputFilePath = path.join(outputDirectory, "include", `scene_${sceneName}.h`);

    SceneDefines.writeToFile(gltf, sceneName, outputFilePath);
}

async function processScene(scene, typeMap, layerMap, archive, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, scene.src);
    const gltfLoader = new GLTFLoader({});

    const n64Scene = await gltfLoader.loadScene(sourceFile, new Map(Object.entries(typeMap)), new Map(Object.entries(layerMap)));
    await SceneWriter.write(n64Scene, outputDirectory, archive);
    writeSceneDefs(scene, gltfLoader.gltf, outputDirectory);
}

module.exports = processScene;