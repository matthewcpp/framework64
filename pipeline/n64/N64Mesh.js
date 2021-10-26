const Bounding = require("./Bounding");

class N64Mesh {
    constructor(name) {
        this.name = name;
        this.primitives = [];
        this.resources = null;
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

    prunePrimitiveVertices() {
        for (const primitive of this.primitives) {
            primitive.pruneUnusedVertices();
        }
    }
}

module.exports = N64Mesh;
