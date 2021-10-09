const N64Defs = require("./N64Defs");

function createVertexBuffer(slices, hasNormals) {
    const vertexList = [];
    for (const slice of slices)
        vertexList.push(...slice.vertices)

    const vertexBuffer = Buffer.alloc(vertexList.length * N64Defs.SizeOfVtx);
    let bufferOffset = 0;

    for (const vertex of vertexList) {
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[0], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[1], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[2], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[3], bufferOffset);

        bufferOffset =  vertexBuffer.writeInt16BE(vertex[4], bufferOffset);
        bufferOffset =  vertexBuffer.writeInt16BE(vertex[5], bufferOffset);

        if (hasNormals) {
            bufferOffset =  vertexBuffer.writeInt8(vertex[6], bufferOffset);
            bufferOffset =  vertexBuffer.writeInt8(vertex[7], bufferOffset);
            bufferOffset =  vertexBuffer.writeInt8(vertex[8], bufferOffset);
        }
        else { // vertex colors
            bufferOffset =  vertexBuffer.writeUInt8(vertex[6], bufferOffset);
            bufferOffset =  vertexBuffer.writeUInt8(vertex[7], bufferOffset);
            bufferOffset =  vertexBuffer.writeUInt8(vertex[8], bufferOffset);
        }

        bufferOffset =  vertexBuffer.writeUInt8(vertex[9], bufferOffset);
    }

    return vertexBuffer;
}

module.exports = {
    createVertexBuffer: createVertexBuffer
};
