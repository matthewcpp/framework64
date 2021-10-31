const GLTFLoader = require("./GLTFLoader");
const MeshWriter = require("./N64MeshWriter");
const Bounding = require("./Bounding")

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

function writeCollisionMeshes(scene, file) {
    const infoBuffer = Buffer.allocUnsafe(16 + Bounding.SizeOf);

    for (const collisionMesh of scene.collisionMeshes) {
        const primitive = collisionMesh.primitives[0];

        let index = 0;
        index = infoBuffer.writeUInt32BE(primitive.vertices.length, index);
        index = infoBuffer.writeUInt32BE(primitive.elements.length * 3, index); // primitive elements are stored as triangles, but we want the total index count
        index = primitive.bounding.writeToBuffer(infoBuffer, index);
        // write empty data for pointers
        index = infoBuffer.writeUInt32BE(0, index);
        index = infoBuffer.writeUInt32BE(0, index);

        const elementDataSize = primitive.elements.length * 3 * 2;
        const vertexDataSize = primitive.vertices.length * 3 * 4;

        const dataBuffer = Buffer.allocUnsafe(elementDataSize + vertexDataSize);
        index = 0;

        for (const vertex of primitive.vertices) {
            for (let i = 0; i < 3; i++) {
                index = dataBuffer.writeFloatBE(vertex[i], index);
            }
        }

        for (const element of primitive.elements) {
            for (const e of element) {
                index = dataBuffer.writeUInt16BE(e, index);
            }
        }

        fs.writeSync(file, infoBuffer);
        fs.writeSync(file, dataBuffer);
    }
}

async function processScene(scene, typeMap, layerMap, archive, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, scene.src);
    const gltfLoader = new GLTFLoader({});

    const n64Scene = await gltfLoader.loadScene(sourceFile, new Map(Object.entries(typeMap)), new Map(Object.entries(layerMap)));

    const destName = path.basename(scene.src, path.extname(scene.src));
    const destFile = path.join(outputDirectory, destName + ".scene");

    const sceneInfo = new SceneInfo(n64Scene);

    const file = fs.openSync(destFile, "w");
    fs.writeSync(file, sceneInfo.buffer);

    await MeshWriter.writeMeshResources(n64Scene.meshResources, file, outputDirectory, archive);

    for (const mesh of n64Scene.meshes) {
        MeshWriter.writeStaticMeshData(mesh, file);
    }

    writeCollisionMeshes(n64Scene, file);
    writeNodes(n64Scene, file);

    fs.closeSync(file);

    await archive.add(destFile, "scene");
}

module.exports = processScene;