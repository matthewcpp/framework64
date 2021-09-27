const Bounding = require("./Bounding");
const N64Material = require("./N64Material");

class N64Mesh {
    constructor(name) {
        this.name = name;
        this.primitives = [];
        this.materials = [];
        this.images = [];
    }

    get bounding() {
        const bounding = new Bounding();

        for (const mesh of this.primitives) {
            bounding.encapsulateBox(mesh.bounding);
        }

        return bounding;
    }

    get hasNormals() {
        for (const mesh of this.primitives) {
            if (mesh.hasNormals)
                return true;
        }
        return false;
    }

    /* determines what shading mode will be used for each material in this mesh */
    setMaterialShadingModes() {
        for (const primitive of this.primitives) {
            const material = this.materials[primitive.material];

            if (primitive.hasNormals) {
                material.shadingMode = material.texture !== N64Material.NoTexture ?
                    N64Material.ShadingMode.GouraudTextured: N64Material.ShadingMode.Gouraud;
            }
            else if (primitive.hasVertexColors) {
                material.shadingMode = N64Material.ShadingMode.UnlitVertexColors;
            }
            else {
                throw new Error(`Could not determine shading mode for primitive in mesh: ${this.name}`);
            }
        }
    }
}

module.exports = N64Mesh;
