const SceneDataWriter = require("../SceneDataWriter");
const MaterialBundleWriter = require("./MaterialBundleWriter");
const MeshWriter = require("./MeshWriter")
const WriteInterface = require("../WriteInterface");

const fs = require("fs");

async function write(scene, gltfData, destPath) {
    const writer = WriteInterface.littleEndian();
    const images = await MaterialBundleWriter.createDesktopImages(gltfData);
    const materialBundle = scene.materialBundle;

    const file = fs.openSync(destPath, "w");

    SceneDataWriter.writeSceneInfo(scene, file, writer);
    if (scene.materialBundle) {
        await MaterialBundleWriter.write(scene.materialBundle, images, gltfData, file);
    }

    for (const gltfMeshIndex of scene.meshBundle) {
        const mesh = gltfData.meshes[gltfMeshIndex];
        await MeshWriter.writeMeshData(mesh, materialBundle, file);
    }

    // The order in which these are written out needs to align with scene.c reading
    SceneDataWriter.writeCollisionMeshes(scene, writer, file);
    SceneDataWriter.writeNodes(scene, writer, file);
    SceneDataWriter.writeCustomBoundingBoxes(scene, writer, file);

    fs.closeSync(file);
}

module.exports = {
    write : write
}