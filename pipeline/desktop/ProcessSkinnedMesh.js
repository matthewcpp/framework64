const path = require("path");
const fs = require("fs");

const Animation = require("../Animation");
const Util = require("../Util");

const {ConvertGltfToGLB} = require('gltf-import-export');

async function processSkinnedMesh(skinnedMesh, bundle, manifestDirectory, outputDirectory) {
    const srcPath = path.join(manifestDirectory, skinnedMesh.src);
    const destName = path.basename(skinnedMesh.src, ".gltf") + ".glb";
    const destPath = path.join(outputDirectory, destName);
    ConvertGltfToGLB(srcPath, destPath);

    const gltf = JSON.parse(fs.readFileSync(srcPath, {encoding: "utf8"}));

    const gltfDir = path.dirname(srcPath);
    const bufferPath = path.join(gltfDir, gltf.buffers[0].uri);
    const buffer = fs.readFileSync(bufferPath);

    const parser = new Animation.Parser();
    const animationData = parser.parse(gltf, buffer, skinnedMesh);

    const writer = new Animation.Writer();
    const meshName = path.basename(srcPath, ".gltf");
    writer.writeLittleEndian(meshName, animationData, outputDirectory);

    bundle.addMesh(skinnedMesh, destName, Object.fromEntries(animationData.jointIdMap));
    bundle.addAnimationData(path.basename(skinnedMesh.src, ".gltf") + ".animation");
}

module.exports = processSkinnedMesh;