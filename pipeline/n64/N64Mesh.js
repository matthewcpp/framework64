const Bounding = require("./Bounding");

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
}

module.exports = N64Mesh;
