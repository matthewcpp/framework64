const Bounding = require("./Bounding");

class N64Primitive {
    static ElementType = {
        Lines: 1,
        Triangles: 4
    };

    static NoMaterial = 0xFFFF;

    vertices = [];
    elements = [];
    elementType;
    texture = null;
    material = 0;
    hasNormals = false;
    hasVertexColors = false;

    bounding = new Bounding();

    constructor(elementType) {
        this.elementType = elementType;
    }
}

module.exports = N64Primitive;