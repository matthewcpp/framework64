const path = require("path");
const GLTFLoader = require("./GLTFLoader");
const MeshWriter = require("./N64MeshWriter");

const Animation = require("../Animation");
const Util = require("../Util");

async function processSkinnedMesh(skinnedMesh, archive, baseDirectory, outputDirectory, includeDirectory) {
    const sourceFile = path.join(baseDirectory, skinnedMesh.src);
    const gltfLoader = new GLTFLoader({});

    let animationOnly = skinnedMesh.hasOwnProperty("animationOnly") ? skinnedMesh.animationOnly : false;

    const meshName = skinnedMesh.hasOwnProperty("name") ? skinnedMesh.name : path.basename(sourceFile, ".gltf");
    const mesh = await gltfLoader.loadStaticMesh(sourceFile);
    mesh.name = meshName; // not totally ideal

    const parser = new Animation.Parser();
    const animationData = parser.parse(gltfLoader.gltf, gltfLoader.loadedBuffers.get(0), skinnedMesh);

    mesh.splitPrimitivesForSkinning();
    mesh.remapJointIndices(animationData.jointIdMap);

    if (!animationOnly) {
        await MeshWriter.writeStaticMesh(mesh, outputDirectory, archive);
    }

    
    const writer = new Animation.Writer();
    writer.writeBigEndian(meshName, animationData, outputDirectory, includeDirectory);

    const safeName = Util.safeDefineName(meshName);
    const destPath = path.join(outputDirectory, safeName + ".animation");
    await archive.add(destPath, "animation_data");
}

module.exports = processSkinnedMesh;