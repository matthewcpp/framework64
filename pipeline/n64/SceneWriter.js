const MeshWriter = require("./N64MeshWriter");
const SceneDataWriter = require("../SceneDataWriter");
const MaterialBundleWriter = require("./MaterialBundleWriter");

const fs = require("fs");
const path = require("path");

const WriteInterface = require("../WriteInterface");

async function write(scene, gltfData, destPath) {
    const writer = WriteInterface.bigEndian();
    const images = await MeshWriter.createN64Images(gltfData);
    const materialBundle = scene.materialBundle;

    const file = fs.openSync(destPath, "w");

    SceneDataWriter.writeSceneInfo(scene, file, writer);
    if (scene.materialBundle) {
        MaterialBundleWriter.write(materialBundle.gltfData, materialBundle, images, file);
    }

    for (const gltfMeshIndex of scene.meshBundle) {
        const mesh = gltfData.meshes[gltfMeshIndex];
        await MeshWriter.writeStaticMeshData(mesh, materialBundle, images, file);
    }

    // The order in which these are written out needs to align with scene.c reading
    SceneDataWriter.writeCollisionMeshes(scene, writer, file);
    SceneDataWriter.writeNodes(scene, writer, file);
    SceneDataWriter.writeCustomBoundingBoxes(scene, writer, file);

    fs.closeSync(file);
}

module.exports = {
    write: write
}
