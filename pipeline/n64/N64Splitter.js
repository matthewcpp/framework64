const N64Primitive = require("./N64Primitive");

function rebuildPrimitives(sourcePrimitive, jointElementMap) {
    const primitives = [];

    jointElementMap.forEach((elements, jointIndex) => {
        const vertexMap = new Map();

        const primitive = new N64Primitive();

        primitive.elementType = sourcePrimitive.elementType;
        primitive.material = sourcePrimitive.material;
        primitive.hasNormals = sourcePrimitive.hasNormals;
        primitive.hasVertexColors = sourcePrimitive.hasVertexColors;
        primitive.jointIndices = [];

        // add all the vertices for this joint to a new primitive
        for (const element of elements) {
            const newElement = [];

            for (const vertex of element) {
                if (vertexMap.has(vertex)) {
                    newElement.push(vertexMap.get(vertex));
                }
                else { // first time seeing this new element, add it to the new primitive and create a mapping for future references
                    const v = sourcePrimitive.vertices[vertex];
                    const newIndex = primitive.vertices.length;
                    vertexMap.set(vertex, newIndex);
                    primitive.vertices.push(v);
                    primitive.jointIndices.push(jointIndex);
                    primitive.bounding.encapsulatePoint(v);
                    newElement.push(newIndex);
                }
            }

            primitive.elements.push(newElement);
        }

        primitives.push(primitive);
    });

    return primitives;
}

function split(primitive) {
    const jointElementMap = new Map();

    for (const element of primitive.elements) {
        const jointIndex = primitive.jointIndices[element[0]];

        if (!jointElementMap.has(jointIndex)) {
            jointElementMap.set(jointIndex, [element]);
        }
        else {
            jointElementMap.get(jointIndex).push(element);
        }
    }

    return rebuildPrimitives(primitive, jointElementMap);
}

module.exports = {
    split: split
}