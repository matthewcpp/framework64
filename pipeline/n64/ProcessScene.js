const GLTFLoader = require("./GLTFLoader");
const MeshWriter = require("./N64MeshWriter");

const path = require("path");
const fs = require("fs");

class SceneInfo {
    nodeCount;
    meshCount;
    colliderCount;
    collisionMeshCount;

    constructor(scene) {
        this.nodeCount = scene.nodes.length;
        this.meshCount = scene.meshes.length;
        this.colliderCount = scene.colliderCount;
        this.collisionMeshCount = scene.collisionMeshes.length;
    }

    get buffer() {
        const buff = Buffer.alloc(16);
        let index = 0;

        index = buff.writeUInt32BE(this.nodeCount, index);
        index = buff.writeUInt32BE(this.meshCount, index);
        index = buff.writeUInt32BE(this.colliderCount, index);
        index = buff.writeUInt32BE(this.collisionMeshCount, index);

        return buff;
    }
}

function writeNodes(scene, file) {
    const transformBuffer = Buffer.allocUnsafe(40);
    const n64MatrixBuffer = Buffer.alloc(64, 0);
    const propertiesBuffer = Buffer.allocUnsafe(16);

    for (const node of scene.nodes) {
        let index = 0;

        // write the transform data
        for (const val of node.position)
            index = transformBuffer.writeFloatBE(val, index)

        for (const val of node.rotation)
            index = transformBuffer.writeFloatBE(val, index)

        for (const val of node.scale)
            index = transformBuffer.writeFloatBE(val, index)

        index = 0

        // write properties data // needs to line up with node.h
        index = propertiesBuffer.writeUInt32BE(node.collider, index);
        index = propertiesBuffer.writeUInt32BE(node.mesh, index);
        index = propertiesBuffer.writeUInt32BE(node.type, index);
        index = propertiesBuffer.writeUInt32BE(node.layerMask, index);

        fs.writeSync(file, transformBuffer);
        fs.writeSync(file, n64MatrixBuffer);
        fs.writeSync(file, propertiesBuffer);
    }
}

async function processScene(scene, typeMap, layerMap, archive, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, scene.src);
    const gltfLoader = new GLTFLoader({});

    const n64Scene = await gltfLoader.loadScene(sourceFile, new Map(Object.entries(typeMap)), new Map(Object.entries(layerMap)));

    const destName = path.basename(scene.src, path.extname(scene.src));
    const destFile = path.join(outputDirectory, destName + ".scene");
    archive.add(destFile, "scene");

    const sceneInfo = new SceneInfo(n64Scene);

    const file = fs.openSync(destFile, "w");
    fs.writeSync(file, sceneInfo.buffer);

    MeshWriter.writeMeshResources(n64Scene.meshResources, file, outputDirectory, archive);

    for (const mesh of n64Scene.meshes) {
        MeshWriter.writeStaticMeshData(mesh, file);
    }

    writeNodes(n64Scene, file);

    fs.closeSync(file);
}

module.exports = processScene;