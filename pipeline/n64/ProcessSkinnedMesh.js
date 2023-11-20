const path = require("path");

const AnimationParser = require("../animation/Parser");
const GLTFLoader = require("../n64/GLTFLoader");
const MaterialBundle = require("../MaterialBundle");
const SkinnedMeshWriter = require("./SkinnedMeshWriter");
const Util = require("../Util");

async function processSkinnedMesh(skinnedMesh, archive, manifestDirectory, outputDirectory, includeDirectory, plugins) {
    const srcPath = path.join(manifestDirectory, skinnedMesh.src);

    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadFile(srcPath);

    if (gltfLoader.meshes.length === 0) {
        throw new Error(`glTF File: ${srcPath} contains no meshes`);
    }

    const parser = new AnimationParser();
    const animationData = parser.parse(gltfLoader.gltf, gltfLoader.loadedBuffers.get(0), skinnedMesh);

    const meshName = Util.safeDefineName(
        Object.hasOwn(skinnedMesh, "name") ? 
        skinnedMesh.name : 
        path.basename(skinnedMesh.src, path.extname(skinnedMesh.src)));

    const mesh = gltfLoader.meshes[0];
    mesh.name = meshName;
    mesh.splitPrimitivesForSkinning();
    mesh.remapJointIndices(animationData.jointIdMap);
    mesh.materialBundle = new MaterialBundle(gltfLoader);
    mesh.materialBundle.bundleMeshMaterials(0);

    plugins.skinnedMeshParsed(skinnedMesh, gltfLoader, animationData);

    const animationOnly = Object.hasOwn(skinnedMesh, "animationOnly") ? skinnedMesh.animationOnly : false;
    const includeFilePath = path.join(includeDirectory, meshName + "_animation.h");

    if (animationOnly) {
        const destFilePath = path.join(outputDirectory, meshName + ".animation");
        SkinnedMeshWriter.writeAimationData(animationData, meshName, destFilePath, includeFilePath);
        bundle.addAnimationData(meshName, destFilePath);
        await archive.add(destFilePath, "animation");
    }
    else {
        const destFilePath = path.join(outputDirectory, meshName + ".skinnedmesh");
        await SkinnedMeshWriter.write(mesh, animationData, destFilePath, includeFilePath);
        await archive.add(destFilePath, "skinnedmesh");
    }
}

module.exports = processSkinnedMesh;
