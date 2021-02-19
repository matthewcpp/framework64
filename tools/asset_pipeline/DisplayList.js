const fs = require("fs")

const G_VTX = 0x01;
const G_TRI1 = 0x05;
const G_TRI2 = 0x06;
const G_LINE3D = 0x08;
const G_ENDDL = 0xdf;

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

const UlongMax = BigInt(0xFFFFFFFF);

function gSPEndDisplayList(gfx) {
    const val = BigInt(_SHIFTL(G_ENDDL, 24, 8));
    gfx.writeUInt32BE(parseInt(val & UlongMax), 0);
    gfx.writeUInt32BE(0, 4);
}

/*
const gfx = Buffer.alloc(8);
const displayList = Buffer.alloc(24);

gSPVertex(gfx, 500, 30, 0);
gfx.copy(displayList, 0, 0, 8);
gSP1Triangle(gfx, 0,1,2, 0)
gfx.copy(displayList, 8, 0, 8);
gSP2Triangles(gfx, 3,4,5,0,6,7,8,0);
gfx.copy(displayList, 16, 0, 8);

fs.writeFileSync("/Users/matthew/development/scratch/dl_test/cmake-build-debug/dl_node.dat", displayList);
*/

module.exports = {
    gSPVertex: gSPVertex,
    gSP1Triangle: gSP1Triangle,
    gSP2Triangles: gSP2Triangles,
    gSPLine3D: gSPLine3D,
    gSPEndDisplayList: gSPEndDisplayList
};