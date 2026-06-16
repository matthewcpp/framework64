const CollisionGeometryWriter = require("../CollisionGeometryWriter");
const MeshWriter = require("./MeshWriter");
const MaterialBundleWriter = require("./MaterialBundleWriter");
const SceneDataWriter = require("../SceneDataWriter");
const WriteInterface = require("../WriteInterface");

const fs = require("fs");

async function write(environment, scene, gltfData, destPath) {
    const file = fs.openSync(destPath, "w");
    await writeToFile(environment, scene, gltfData, file);
    fs.closeSync(file);
}

async function writeToFile(environment, scene, gltfData, file) {
    // TODO: can we pass in writer/images?
    const writer = WriteInterface.bigEndian();
    const images = await MeshWriter.createN64Images(gltfData);
    const materialBundle = scene.materialBundle;

    SceneDataWriter.writeSceneInfo(scene, file, writer);
    if (scene.materialBundle) {
        MaterialBundleWriter.write(materialBundle.gltfData, materialBundle, images, file);
    }

    if (scene.collisionGeometry) {
        const collisionGeometryWriter = new CollisionGeometryWriter(writer);
        collisionGeometryWriter.writeData(scene.collisionGeometry, file);
    }

    for (const gltfMeshIndex of scene.meshBundle) {
        const mesh = gltfData.meshes[gltfMeshIndex];
        await MeshWriter.writeStaticMeshData(mesh, materialBundle, images, file);
    }

    // The order in which these are written out needs to align with scene.c reading
    SceneDataWriter.writeCollisionMeshes(environment, scene, writer, file);
    SceneDataWriter.writeNodes(environment, scene, writer, file);
    SceneDataWriter.writeCustomBoundingBoxes(scene, writer, file);
}

module.exports = {
    write: write,
    writeToFile: writeToFile
}
