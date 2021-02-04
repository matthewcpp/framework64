const Bounding = require("./Bounding");
const N64Defs = require("./N64Defs");

class MeshSlice {
    constructor() {
        /** Maps the raw vertex index to its index within this slice */
        this.vertexIndices = new Map();
        this.triangles = [];
    }

    _getNeededVertices(triangle) {
        const neededVertices = [];

        for (const index of triangle) {
            if (!this.vertexIndices.has(index))
                neededVertices.push(index);
        }

        return neededVertices;
    }

    addTriangle(triangle) {
        const neededVertices = this._getNeededVertices(triangle);

        if (neededVertices.length <= N64Defs.vertexSliceSize - this.vertexIndices.size) {
            for (const index of neededVertices)
                this.vertexIndices.set(index, this.vertexIndices.size);

            this.triangles.push(triangle);

            return true;
        }
        else {
            return false;
        }
    }
}

class N64Mesh {
    vertices = [];
    triangles = [];
    texture = null;
    material = null;
    hasNormals = false;
    hasTexCoords = false;

    bounding = new Bounding();

    _addTriangleToSlice(triangle, slices) {
        for (const slice of slices) {
            if (slice.addTriangle(triangle))
                return true;
        }

        return false;
    }

    _partitionVertices() {
        const slices = [new MeshSlice()];

        // parition all triangles and their vertices into slices
        for (const triangle of this.triangles) {
            if (this._addTriangleToSlice(triangle, slices))
                continue; // triangle fit in existing slice

            const slice = new MeshSlice();
            slice.addTriangle(triangle);
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
            const triangleList = []
            for (const triangle of slice.triangles) {
                triangleList.push([
                    slice.vertexIndices.get(triangle[0]), slice.vertexIndices.get(triangle[1]), slice.vertexIndices.get(triangle[2])
                ])
            }

            processed.push({vertices: vertexList, triangles: triangleList})
        }

        return processed;
    }

    slice() {
        // if the mesh is small, then there is only one slice
        if (this.vertices.length <= N64Defs.vertexSliceSize) {
            return [{vertices: this.vertices, triangles: this.triangles}]
        }
        else {
            return this._processSlices(this._partitionVertices());
        }
    }
}

module.exports = N64Mesh;