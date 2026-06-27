const MaterialBundle = require("./gltf/MaterialBundle");
const GLTFLoader = require("./gltf/GLTFLoader");
const MeshWriter = require("./desktop/MeshWriter");
const MeshCustomBounding = require("./gltf/MeshCustomBounding");

const path = require("path");


class GltfMeshProcessor {
    _environment;
    _meshWriter;
    constructor(environment, meshWriter) {
        this._environment = environment;
        this._meshWriter = meshWriter;
    }

    async process(meshJson) {
        const srcPath = path.join(this._environment.assetDirectory, meshJson.src);
        const gltfLoader = new GLTFLoader();
        await gltfLoader.loadFile(srcPath);

        if (!gltfLoader.gltf.meshes || gltfLoader.gltf.meshes.length === 0) {
            throw new Error(`glTF File: ${gltfPath} contains no meshes`);
        }

        const staticMesh = gltfLoader.meshes[0];
        MeshCustomBounding.setForStaticMesh(staticMesh, gltfLoader);

        const meshName = !!meshJson.name ? meshJson.name : path.basename(meshJson.src, path.extname(meshJson.src));
        staticMesh.name = meshName;

        staticMesh.materialBundle = new MaterialBundle(gltfLoader);
        staticMesh.materialBundle.bundleMeshMaterials(0);

        const assetFileName = staticMesh.name + ".mesh";
        const destPath = path.join(this._environment.outputDirectory, assetFileName);
        await this._meshWriter.writeStaticMesh(this._environment, staticMesh, destPath);

        this._environment.assetBundle.addMesh(assetFileName, meshName);
    }
};

module.exports = GltfMeshProcessor;