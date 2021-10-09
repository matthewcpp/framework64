const GLTFLoader = require("./GLTFLoader");
const MeshWriter = require("./N64MeshWriter")

const path = require("path");
const fs = require("fs");

class TerrainInfo {
    meshCount;

    get buffer() {
        const buff = Buffer.alloc(4);
        let index = 0;

        index = buff.writeUInt32BE(this.meshCount, index);

        return buff;
    }
}

async function processTerrain(terrain, archive, baseDirectory, outputDirectory) {
    const sourceFile = path.join(baseDirectory, terrain.src);
    const gltfLoader = new GLTFLoader({});

    const {meshes, resources} = await gltfLoader.loadTerrain(sourceFile);

    const destName = path.basename(terrain.src, path.extname(terrain.src));
    const destFile = path.join(outputDirectory, destName + ".terrain");
    archive.add(destFile, "terrain");

    const file = fs.openSync(destFile, "w");
    MeshWriter.writeMeshResources(resources, file, outputDirectory, archive);

    const terrainInfo = new TerrainInfo();
    terrainInfo.meshCount = meshes.length;

    fs.writeSync(file, terrainInfo.buffer);

    for (const mesh of meshes) {
        MeshWriter.writeStaticMeshData(mesh, file);
    }

    fs.closeSync(file);
}

module.exports = processTerrain;