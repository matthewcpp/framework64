const CollisionGeometryWriter = require("./CollisionGeometryWriter");
const SceneDataWriter = require("./SceneDataWriter");
const WriteInterface = require("../WriteInterface");

const fs = require("fs");

async function write(environment, scene, gltfData, destPath, materialBundleWriter, meshWriter) {
    const file = fs.openSync(destPath, "w");
    await writeToFile(environment, scene, gltfData, file, materialBundleWriter, meshWriter);
    fs.closeSync(file);
}

async function writeToFile(environment, scene, gltfData, file, materialBundleWriter, meshWriter) {
    const writer = environment.binaryWriter;
    const images = await materialBundleWriter.createImages(gltfData);
    const materialBundle = scene.materialBundle;

    SceneDataWriter.writeSceneInfo(scene, file, writer);
    if (scene.materialBundle) {
        await materialBundleWriter.write(scene.materialBundle, images, gltfData, file);
    }

    if (scene.collisionGeometry) {
        const collisionGeometryWriter = new CollisionGeometryWriter(writer);
        collisionGeometryWriter.writeData(scene.collisionGeometry, file);
    }

    for (const gltfMeshIndex of scene.meshBundle) {
        const mesh = gltfData.meshes[gltfMeshIndex];
        await meshWriter.writeMeshData(environment, mesh, materialBundle, images, file);
    }

    // The order in which these are written out needs to align with scene.c reading
    SceneDataWriter.writeCollisionMeshes(environment, scene, writer, file);
    SceneDataWriter.writeNodes(environment, scene, writer, file);
    SceneDataWriter.writeCustomBoundingBoxes(scene, writer, file);
}

module.exports = {
    write : write,
    writeToFile: writeToFile
}
