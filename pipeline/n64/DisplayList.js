const N64Defs = require("./N64Defs")

const G_VTX = 0x01;
const G_TRI1 = 0x05;
const G_TRI2 = 0x06;
const G_LINE3D = 0x08;
const G_ENDDL = 0xdf;

const UlongMax = BigInt(0xFFFFFFFF);

function _SHIFTL(v, s, w) {
    return ((((v) & ((0x01 << (w)) - 1)) << (s)));
}

function __gsSPLine3D_w1(v0, v1, wd) {
    return (_SHIFTL((v0)*2,16,8)|_SHIFTL((v1)*2,8,8)|_SHIFTL((wd),0,8));
}

function __gsSPLine3D_w1f(v0, v1, wd, flag) {
    return (flag === 0) ? __gsSPLine3D_w1(v0, v1, wd) : __gsSPLine3D_w1(v1, v0, wd);
}

function __gsSP1Triangle_w1(v0, v1, v2) {
    return (_SHIFTL((v0)*2,16,8)|_SHIFTL((v1)*2,8,8)|_SHIFTL((v2)*2,0,8));
}

function __gsSP1Triangle_w1f(v0, v1, v2, flag) {
        return (((flag) === 0) ? __gsSP1Triangle_w1(v0, v1, v2) :
            ((flag) === 1) ? __gsSP1Triangle_w1(v1, v2, v0) :
                __gsSP1Triangle_w1(v2, v0, v1));
}

function gSPVertex(gfx, v, n, v0) {
    gfx.writeUInt32BE((_SHIFTL(G_VTX,24,8)|_SHIFTL((n),12,8)|_SHIFTL((v0)+(n),1,7)), 0);
    gfx.writeUInt32BE(v, 4);
}

function gSP1Triangle(gfx, v0, v1, v2, flag) {
    gfx.writeUInt32BE(_SHIFTL(G_TRI1, 24, 8)|__gsSP1Triangle_w1f(v0, v1, v2, flag), 0);
    gfx.writeUInt32BE(0, 4);
}

function gSP2Triangles(gfx, v00, v01, v02, flag0, v10, v11, v12, flag1) {
    gfx.writeUInt32BE(_SHIFTL(G_TRI2, 24, 8)| __gsSP1Triangle_w1f(v00, v01, v02, flag0), 0);
    gfx.writeUInt32BE(__gsSP1Triangle_w1f(v10, v11, v12, flag1), 4);
}

function gSPLine3D(gfx, v0, v1, flag) {
    gfx.writeUInt32BE(_SHIFTL(G_LINE3D, 24, 8)|__gsSPLine3D_w1f(v0, v1, 0, flag), 0);
    gfx.writeUInt32BE(0, 4);
}

function gSPEndDisplayList(gfx) {
    const val = BigInt(_SHIFTL(G_ENDDL, 24, 8));
    gfx.writeUInt32BE(parseInt(val & UlongMax), 0);
    gfx.writeUInt32BE(0, 4);
}

function createTriangleDisplayListBuffer(slices) {
    let displayListSize = 1; // gSPEndDisplayList
    for (const slice of slices) {
        displayListSize += 1 + slice.elements.length / 2 + slice.elements.length % 2;
    }

    const gfx = Buffer.alloc(8);

    const displayListBuffer = Buffer.alloc(displayListSize * N64Defs.SizeOfGfx);
    let displayListIndex = 0; // holds the current index into the display list array.

    const vertexPointerBuffer = Buffer.alloc(slices.length * 4); // holds indices of this primitive's display list that contain vertex cache pointers (gSPVertex)
    let vertexPointerBufferIndex = 0;

    let vertexBufferOffset = 0; // holds the offset into the vertex buffer for this primitive.  Incremented every slice

    for (const slice of slices) {
        // writes the index of the gSPVertex call of the display list into the list of items that need to be fixed up when loaded at runtime
        vertexPointerBufferIndex = vertexPointerBuffer.writeInt32BE(displayListIndex, vertexPointerBufferIndex);

        // writes the relative offset into the vertex buffer for this slice.  At runtime the base address of the primitive's vertex buffer will be added to this value
        gSPVertex(gfx, vertexBufferOffset, slice.vertices.length, 0)
        gfx.copy(displayListBuffer, displayListIndex++ * N64Defs.SizeOfGfx);

        for (let i = 0; i < slice.elements.length - slice.elements.length % 2; i+= 2) {
            const t1 = slice.elements[i];
            const t2 = slice.elements[i + 1];

            gSP2Triangles(gfx, t1[0],t1[1],t1[2],0,t2[0],t2[1],t2[2],0);
            gfx.copy(displayListBuffer, displayListIndex++ * N64Defs.SizeOfGfx);
        }

        if (slice.elements.length % 2 !== 0) {
            const t1 = slice.elements[slice.elements.length - 1];
            gSP1Triangle(gfx, t1[0],t1[1],t1[2],0);
            gfx.copy(displayListBuffer, displayListIndex++ * N64Defs.SizeOfGfx);
        }

        vertexBufferOffset += slice.vertices.length * N64Defs.SizeOfVtx;
    }

    gSPEndDisplayList(gfx);
    gfx.copy(displayListBuffer, displayListIndex++ * N64Defs.SizeOfGfx);

    return {
        displayList: displayListBuffer,
        vertexPointers: vertexPointerBuffer,
    }
}

function createLineDisplayListBuffer(slices) {
    let displayListSize = 1; // gSPEndDisplayList
    for (const slice of slices) {
        displayListSize += 1 + slice.elements.length;
    }

    const gfx = Buffer.alloc(8);

    const displayListBuffer = Buffer.alloc(displayListSize * N64Defs.SizeOfGfx);
    let displayListIndex = 0; // holds the current index into the display list array.

    const vertexPointerBuffer = Buffer.alloc(slices.length * 4); // holds indices of this primitive's display list that contain vertex cache pointers (gSPVertex)
    let vertexPointerBufferIndex = 0;

    let vertexBufferOffset = 0; // holds the offset into the vertex buffer for this primitive.  Incremented every slice

    for (const slice of slices) {
        // writes the index of the gSPVertex call of the display list into the list of items that need to be fixed up when loaded at runtime
        vertexPointerBufferIndex = vertexPointerBuffer.writeInt32BE(displayListIndex, vertexPointerBufferIndex);

        // writes the relative offset into the vertex buffer for this slice.  At runtime the base address of the primitive's vertex buffer will be added to this value
        gSPVertex(gfx, vertexBufferOffset, slice.vertices.length, 0)
        gfx.copy(displayListBuffer, displayListIndex++ * N64Defs.SizeOfGfx);

        for (const line of slice.elements) {
            gSPLine3D(gfx, line[0], line[1], 0);
            gfx.copy(displayListBuffer, displayListIndex++ * N64Defs.SizeOfGfx);
        }

        vertexBufferOffset += slice.vertices.length * N64Defs.SizeOfVtx;
    }

    gSPEndDisplayList(gfx);
    gfx.copy(displayListBuffer, displayListIndex++ * N64Defs.SizeOfGfx);

    return {
        displayList: displayListBuffer,
        vertexPointers: vertexPointerBuffer,
    }
}

module.exports = {
    createTriangleDisplayListBuffer: createTriangleDisplayListBuffer,
    createLineDisplayListBuffer: createLineDisplayListBuffer
};
