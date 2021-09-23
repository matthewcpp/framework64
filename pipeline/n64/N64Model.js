const Bounding = require("./Bounding");

class N64Model {
    constructor(name) {
        this.name = name;
        this.meshes = [];
        this.materials = [];
        this.images = [];
    }

    get bounding() {
        const bounding = new Bounding();

        for (const mesh of this.meshes) {
            bounding.encapsulateBox(mesh.bounding);
        }

        return bounding;
    }

    get hasNormals() {
        for (const mesh of this.meshes) {
            if (mesh.hasNormals)
                return true;
        }
        return false;
    }
}

module.exports = N64Model;
