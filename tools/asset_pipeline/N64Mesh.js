const Bounding = require("./Bounding");
const N64Defs = require("./N64Defs");

class N64Mesh {
    constructor() {
        this.vertices = [];
        this.triangles = [];
        this.texture = null;

        this.bounding = new Bounding();
    }

    slice() {
        // if the mesh is small, then there is only one slice
        if (this.vertices.length <= N64Defs.vertexSliceSize) {
            return [{vertices: this.vertices, triangles: this.triangles}]
        }

        const slices = [];

        // parition all vertices into their slices
        for (let i = 0; i < this.vertices.length; i++) {
            if (i % N64Defs.vertexSliceSize === 0) {
                slices.push({vertices: [], triangles: []});
            }

            slices[slices.length - 1].vertices.push(this.vertices[i]);
        }

        const getTriangleSliceIndex = (triangle) => {
            let index = -1;
            for (let i = 0; i < slices.length; i++) {
                const minIndex = i * N64Defs.vertexSliceSize;
                const maxIndex = (i + 1) * N64Defs.vertexSliceSize;

                // triangle fits in this slice?
                if ((triangle[0] >= minIndex && triangle[0] < maxIndex) &&
                    (triangle[1] >= minIndex && triangle[1] < maxIndex) &&
                    (triangle[2] >= minIndex && triangle[2] < maxIndex)) {
                        index = i;
                        break;
                    }
            }

            return index;
        }

        // sort all triangles into their slices
        for (const triangle of this.triangles) {
            const sliceIndex = getTriangleSliceIndex(triangle);
            if (sliceIndex === -1) {
                continue;
            }
            // adjust triangle indices and add to triangle list
            slices[sliceIndex].triangles.push(
                [triangle[0] - sliceIndex * N64Defs.vertexSliceSize, 
                triangle[1] - sliceIndex * N64Defs.vertexSliceSize, 
                triangle[2] - sliceIndex * N64Defs.vertexSliceSize]);
        }

        return slices;
    }
}

module.exports = N64Mesh;