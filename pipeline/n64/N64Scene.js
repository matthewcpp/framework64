const MaterialBundle = require("../MaterialBundle");

/** This is a platform independant container for scene data */
class N64Scene {
    name = "scene"
    nodes = [];
    colliderCount = 0;
    collisionMeshes = [];
    customBoundingBoxes = [];
    materialBundle = null;

    meshBundle = [];

    /** Gets or assigns a bundled mesh index for the GLTF mesh index in this scene */
    bundleMeshIndex(gltfMeshIndex) {
        let bundledIndex = this.getBundledMeshIndex(gltfMeshIndex);

        if (bundledIndex < 0) {
            bundledIndex = this.meshBundle.length;
            this.meshBundle.push(gltfMeshIndex);

            if (this.materialBundle) {
                this.materialBundle.bundleMeshMaterials(gltfMeshIndex);
            }
        }

        return bundledIndex;
    }

    getBundledMeshIndex(gltfMeshIndex) {
        return this.meshBundle.indexOf(gltfMeshIndex);
    }
}

module.exports = N64Scene;