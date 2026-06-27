const AnimationParser = require("./animation/Parser");
const GLTFLoader = require("./gltf/GLTFLoader");
const MaterialBundle = require("./gltf/MaterialBundle");
const SkinnedMeshWriter = require("./SkinnedMeshWriter");
const Util = require("./Util");

const path = require("path");
const MeshCustomBounding = require("./gltf/MeshCustomBounding");

class GltfSkinnedMeshProcessor {
    _environment;
    _meshWriter;

    constructor(environment, meshWriter) {
        this._environment = environment;
        this._meshWriter = meshWriter;
    }

    async process(skinnedMesh) {
        const srcPath = path.join(this._environment.assetDirectory, skinnedMesh.src);

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
        MeshCustomBounding.setForStaticMesh(mesh, gltfLoader);

        mesh.name = meshName;
        mesh.splitPrimitivesForSkinning();
        mesh.remapJointIndices(animationData.jointIdMap);
        mesh.materialBundle = new MaterialBundle(gltfLoader);
        mesh.materialBundle.bundleMeshMaterials(0);

        const animationOnly = Object.hasOwn(skinnedMesh, "animationOnly") ? skinnedMesh.animationOnly : false;
        const includeFilePath = path.join(this._environment.includeDirectory, meshName + "_animation.h");

        if (animationOnly) {
            const destFilePath = path.join(this._environment.outputDirectory, meshName + ".animation");
            SkinnedMeshWriter.writeAnimationData(this._environment, animationData, meshName, destFilePath, includeFilePath);
            environment.assetBundle.addAnimationData(destFilePath, meshName);
        }
        else {
            const destFilePath = path.join(this._environment.outputDirectory, meshName + ".skinnedmesh");
            await SkinnedMeshWriter.write(this._environment, mesh, animationData, destFilePath, includeFilePath, this._meshWriter);
            this._environment.assetBundle.addSkinnedMesh(destFilePath, meshName);
        }
    }
};

module.exports = GltfSkinnedMeshProcessor;
