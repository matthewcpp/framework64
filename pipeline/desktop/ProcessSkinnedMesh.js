const path = require("path");
const fs = require("fs");

const Animation = require("../Animation");
const Util = require("../Util");

const {ConvertGltfToGLB} = require('gltf-import-export');

async function processSkinnedMesh(skinnedMesh, bundle, manifestDirectory, outputDirectory, includeDirectory, plugins) {
    const srcPath = path.join(manifestDirectory, skinnedMesh.src);
    const meshName = skinnedMesh.hasOwnProperty("name") ? skinnedMesh.name : path.basename(skinnedMesh.src, path.extname(skinnedMesh.src));
    const destName = meshName + ".glb";
    const destPath = path.join(outputDirectory, destName);
    

    const gltf = JSON.parse(fs.readFileSync(srcPath, {encoding: "utf8"}));

    const gltfDir = path.dirname(srcPath);
    const bufferPath = path.join(gltfDir, gltf.buffers[0].uri);
    const buffer = fs.readFileSync(bufferPath);

    const parser = new Animation.Parser();
    const animationData = parser.parse(gltf, buffer, skinnedMesh);

    const writer = new Animation.Writer();
    writer.writeLittleEndian(meshName, animationData, outputDirectory, includeDirectory);

    let animationOnly = skinnedMesh.hasOwnProperty("animationOnly") ? skinnedMesh.animationOnly : false;
    if (!animationOnly) {
        ConvertGltfToGLB(srcPath, destPath);
        bundle.addMesh(meshName, destName, Object.fromEntries(animationData.jointIdMap));
    }
    
    bundle.addAnimationData(meshName + ".animation");
}

module.exports = processSkinnedMesh;