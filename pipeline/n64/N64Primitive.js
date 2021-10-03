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
    material = 0;
    hasNormals = false;
    hasVertexColors = false;

    bounding = new Bounding();

    constructor(elementType) {
        this.elementType = elementType;
    }

    pruneUnusedVertices() {
        const elementMap = new Map();
        const newVertices = [];
        const newElements = [];

        for (const triangle of this.elements) {
            const newTriangle = [];

            for (const vertex of triangle) {
                if (elementMap.has(vertex)) {
                    newTriangle.push(elementMap.get(vertex));
                }
                else {
                    const newVertexIndex = newVertices.length;
                    elementMap.set(vertex, newVertexIndex);
                    newVertices.push(this.vertices[vertex]);
                    newTriangle.push(newVertexIndex);
                }
            }

            newElements.push(newTriangle);
        }

        this.vertices = newVertices;
        this.elements = newElements;
    }
}

module.exports = N64Primitive;