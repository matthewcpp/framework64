class MaterialBundle {
    gltfData = null;
    images = [];
    textures = [];
    materials = [];

    constructor(gltfData) {
        this.gltfData = gltfData;

        if (!this.gltfData) {
            throw new Error("Material Bundle requires non-null GLTF data");
        }
    }

    bundleMeshMaterials(gltfMeshIndex) {
        const mesh = this.gltfData.meshes[gltfMeshIndex];
        for (const primtivie of mesh.primitives) {
            this.bundleMaterial(primtivie.material);
        }
    }

    /** Gets or creates a bundled material index for a index into the gltf material array */
    bundleMaterial(gltfMaterialIndex) {
        let bundledIndex = this.getBundledMaterialIndex(gltfMaterialIndex)
        if (bundledIndex < 0) {
            bundledIndex = this.materials.length;
            this.materials.push(gltfMaterialIndex);
            const material = this.gltfData.materials[gltfMaterialIndex];

            if (material.hasTexture()) {
                this.bundleTexture(material.texture);
            }
        }

        return bundledIndex;
    }

    /** Gets or creates a bundled texture index for a index into the gltf texture array */
    bundleTexture(gltfTextureIndex) {
        let bundledIndex = this.getBundledTextureIndex(gltfTextureIndex);
        if (bundledIndex < 0) {
            bundledIndex = this.textures.length;
            this.textures.push(gltfTextureIndex);

            const texture = this.gltfData.textures[gltfTextureIndex];
            this.bundleImage(texture.image);
        }

        return bundledIndex;
    }

    /** Gets or creates a bundled image index for a index into the gltf image array */
    bundleImage(gltfImageIndex) {
        let bundledIndex = this.getBundledImageIndex(gltfImageIndex);

        if (bundledIndex < 0) {
            bundledIndex = this.images.length;
            this.images.push(gltfImageIndex);
        }

        return bundledIndex;
    }

    /** Returns the bundled image index for the index into the GLTF image array */
    getBundledImageIndex(gltfImageIndex) {
        return this.images.indexOf(gltfImageIndex)
    }

    /** Returns the bundled texture index for the index into the GLTF texture array */
    getBundledTextureIndex(gltfTextureIndex) {
        return this.textures.indexOf(gltfTextureIndex);
    }

    /** Returns the bundled material index for the index into the GLTF material array */
    getBundledMaterialIndex(gltfMaterialIndex) {
        return this.materials.indexOf(gltfMaterialIndex);
    }
}

module.exports = MaterialBundle;
