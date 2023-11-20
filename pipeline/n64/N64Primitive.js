const Bounding = require("./Bounding");

class N64Primitive {
    static ElementType = {
        Lines: 1,
        Triangles: 4
    };

    static NoMaterial = 0xFFFF;
    static NoJoint = 0xFFFF;
    static InvalidMaterialIndex = -1;

    vertices = [];
    jointIndices = null;
    elements = [];

    /// This should be renamed to primitive mode
    elementType;

    /** index of the material for this primitive */
    material = 0;
    hasPositions = false;
    hasNormals = false;
    hasVertexColors = false;
    hasTexCoords = false;

    bounding = new Bounding();

    constructor(elementType) {
        this.elementType = elementType;
    }

    get containsMultipleJointIndices () {
        if (this.jointIndices == null)
            return false;

        const jointIndex = this.jointIndices[0];
        for (let i = 0; i < this.jointIndices.length; i++) {
            if (this.jointIndices[i] !== jointIndex)
                return true;
        }

        return false;
    }

    pruneUnusedVertices() {
        this.bounding = new Bounding();
        const elementMap = new Map();
        const newVertices = [];
        const newElements = [];
        const newJointIndices = this.jointIndices ? [] : null;

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

                    if (newJointIndices) {
                        newJointIndices.push(this.jointIndices[vertex]);
                    }

                    this.bounding.encapsulatePoint(this.vertices[vertex]);
                    newTriangle.push(newVertexIndex);
                }
            }

            newElements.push(newTriangle);
        }

        this.vertices = newVertices;
        this.elements = newElements;
        this.jointIndices = newJointIndices;
    }
}

module.exports = N64Primitive;