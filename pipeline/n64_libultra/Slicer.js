const N64Defs = require("./N64Defs");

class PrimitiveSlice {
    vertices;
    elements;

    constructor(vertices, elements) {
        this.vertices = vertices;
        this.elements = elements;
    }
}

class VertexBufferPartition {
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

function addTriangleToPartition(triangle, partitions) {
    for (const partition of partitions) {
        if (partition.addElement(triangle))
            return true;
    }

    return false;
}
function partitionVertices(primitive) {
    const partitions = [new VertexBufferPartition()];

    // partition all triangles and their vertices into n64 Sized buckets
    for (const element of primitive.elements) {
        if (addTriangleToPartition(element, partitions))
            continue; // triangle fit in existing partition

        // could not find a partition that had room for this triangle, make a new one and add it
        const partition = new VertexBufferPartition();
        partition.addElement(element);
        partitions.push(partition);
    }

    return partitions;
}

function createSlicesFromPartitions(primitive, partitions) {
    //build vertex and triangle list for each slice
    const processed = [];

    for (const partition of partitions) {
        // create vertex list for this slice by grabbing the original mesh vertex and placing it into the slice aray
        const vertexList = new Array(partition.vertexIndices.size);
        partition.vertexIndices.forEach((position, index)=> {
            vertexList[position] = primitive.vertices[index];
        });

        // build updated triangle list by mapping the original vertex index to its index in this slice.
        const elementList = []
        for (const triangle of partition.elements) {
            elementList.push(triangle.map((val) => partition.vertexIndices.get(val)));
        }

        processed.push(new PrimitiveSlice(vertexList, elementList));
    }

    return processed;
}

function slice(primitive) {
    if (primitive.vertices.length < N64Defs.vertexSliceSize) {
        return [new PrimitiveSlice(primitive.vertices, primitive.elements)];
    }
    else {
        return createSlicesFromPartitions(primitive, partitionVertices(primitive));
    }
}

module.exports = {
    slice: slice
}
