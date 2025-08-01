const CollisionGeometryWriter = require("../CollisionGeometryWriter");
const MaterialBundleWriter = require("./MaterialBundleWriter");
const MeshWriter = require("./MeshWriter")
const SceneDataWriter = require("../SceneDataWriter");
const WriteInterface = require("../WriteInterface");

const fs = require("fs");

async function write(environment, scene, gltfData, destPath) {
    const writer = WriteInterface.littleEndian();
    const images = await MaterialBundleWriter.createDesktopImages(gltfData);
    const materialBundle = scene.materialBundle;

    const file = fs.openSync(destPath, "w");

    SceneDataWriter.writeSceneInfo(scene, file, writer);
    if (scene.materialBundle) {
        await MaterialBundleWriter.write(scene.materialBundle, images, gltfData, file);
    }

    if (scene.collisionGeometry) {
        const collisionGeometryWriter = new CollisionGeometryWriter(writer);
        collisionGeometryWriter.writeData(scene.collisionGeometry, file);
    }

    for (const gltfMeshIndex of scene.meshBundle) {
        const mesh = gltfData.meshes[gltfMeshIndex];
        await MeshWriter.writeMeshData(environment, mesh, materialBundle, file);
    }

    // The order in which these are written out needs to align with scene.c reading
    SceneDataWriter.writeCollisionMeshes(environment, scene, writer, file);
    SceneDataWriter.writeNodes(environment, scene, writer, file);
    SceneDataWriter.writeCustomBoundingBoxes(scene, writer, file);

    fs.closeSync(file);
}

module.exports = {
    write : write
}