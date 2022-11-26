const path = require("path");
const fse = require("fs-extra");

const Animation = require("../Animation");
const GLTFLoader = require("../n64/GLTFLoader");

const {ConvertGltfToGLB} = require('gltf-import-export');

async function processSkinnedMesh(skinnedMesh, bundle, manifestDirectory, outputDirectory, includeDirectory, plugins) {
    const srcPath = path.join(manifestDirectory, skinnedMesh.src);
    const meshName = skinnedMesh.hasOwnProperty("name") ? skinnedMesh.name : path.basename(skinnedMesh.src, path.extname(skinnedMesh.src));

    const gltfLoader = new GLTFLoader();
    await gltfLoader.loadStaticMesh(srcPath);

    const parser = new Animation.Parser();
    const animationData = parser.parse(gltfLoader.gltf, gltfLoader.loadedBuffers.get(0), skinnedMesh);

    plugins.skinnedMeshParsed(skinnedMesh, gltfLoader, animationData);

    let animationOnly = skinnedMesh.hasOwnProperty("animationOnly") ? skinnedMesh.animationOnly : false;
    if (!animationOnly) {
        // temporary until desktop asset rework
        const gltfDirName = path.dirname(skinnedMesh.src);
        const gltfSrcDir = path.join(manifestDirectory, gltfDirName);
        const gltfDestDir = path.join(outputDirectory, gltfDirName);
        fse.copySync(gltfSrcDir, gltfDestDir);
        bundle.addMesh(meshName, skinnedMesh.src, Object.fromEntries(animationData.jointIdMap));
    }

    const writer = new Animation.Writer();
    writer.writeLittleEndian(meshName, animationData, outputDirectory, includeDirectory);
    bundle.addAnimationData(meshName + ".animation");

}

module.exports = processSkinnedMesh;