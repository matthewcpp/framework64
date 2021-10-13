const GLTFLoader = require("./GLTFLoader");
const MeshWriter = require("./N64MeshWriter");
const ExtrasWriter = require("./N64ExtrasWriter");

const path = require("path");
const fs = require("fs");

class SceneInfo {
    meshCount;
    extraCount;

    get buffer() {
        const buff = Buffer.alloc(8);
        let index = 0;

        index = buff.writeUInt32BE(this.meshCount, index);
        index = buff.writeUInt32BE(this.extraCount, index);

        return buff;
    }
}

async function processScene(scene, typemap, archive, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, scene.src);
    const gltfLoader = new GLTFLoader({});

    const {meshes, resources, extras} = await gltfLoader.loadScene(sourceFile);

    const destName = path.basename(scene.src, path.extname(scene.src));
    const destFile = path.join(outputDirectory, destName + ".scene");
    archive.add(destFile, "scene");

    const sceneInfo = new SceneInfo();
    sceneInfo.meshCount = meshes.length;
    sceneInfo.extraCount = extras.length;

    const file = fs.openSync(destFile, "w");
    fs.writeSync(file, sceneInfo.buffer);

    MeshWriter.writeMeshResources(resources, file, outputDirectory, archive);

    for (const mesh of meshes) {
        MeshWriter.writeStaticMeshData(mesh, file);
    }

    ExtrasWriter.write(extras, typemap, file);

    fs.closeSync(file);
}

module.exports = processScene;