const Bounding = require("./Bounding");
const N64Defs = require("./N64Defs");

class MeshSlice {
    constructor() {
        /** Maps the raw vertex index to its index within this slice */
        this.vertexIndices = new Map();
        this.elements = [];
    }

    _getNeededVertices(element) {
        const neededVertices = [];

        for (const index of element) {
            if (!this.vertexIndices.has(index))
                neededVertices.push(index);
        }

        return neededVertices;
    }

    addElement(element) {
        const neededVertices = this._getNeededVertices(element);

        if (neededVertices.length <= N64Defs.vertexSliceSize - this.vertexIndices.size) {
            for (const index of neededVertices)
                this.vertexIndices.set(index, this.vertexIndices.size);

            this.elements.push(element);

            return true;
        }
        else {
            return false;
        }
    }
}

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

    _addTriangleToSlice(triangle, slices) {
        for (const slice of slices) {
            if (slice.addElement(triangle))
                return true;
        }

        return false;
    }

    _partitionVertices() {
        const slices = [new MeshSlice()];

        // parition all triangles and their vertices into slices
        for (const element of this.elements) {
            if (this._addTriangleToSlice(element, slices))
                continue; // triangle fit in existing slice

            const slice = new MeshSlice();
            slice.addElement(element);
            slices.push(slice);
        }

        return slices;
    }

    _processSlices(slices) {
        //build vertex and triangle list for each slice
        const processed = [];

        for (const slice of slices) {
            // create vertex list for this slice by grabbing the original mesh vertex and placing it into the slice aray
            const vertexList = new Array(slice.vertexIndices.size);
            slice.vertexIndices.forEach((position, index)=> {
                vertexList[position] = this.vertices[index];
            });

            // build updated triangle list by mapping the original vertex index to its index in this slice.
            const elementList = []
            for (const triangle of slice.elements) {
                elementList.push(triangle.map((val) => slice.vertexIndices.get(val)));
            }

            processed.push({vertices: vertexList, elements: elementList})
        }

        return processed;
    }

    slice() {
        // if the mesh is small, then there is only one slice
        if (this.vertices.length <= N64Defs.vertexSliceSize) {
            return [{vertices: this.vertices, elements: this.elements}]
        }
        else {
            return this._processSlices(this._partitionVertices());
        }
    }
}

module.exports = N64Primitive;