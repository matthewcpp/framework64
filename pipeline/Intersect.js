const glMatrix = require("gl-matrix");

const right = glMatrix.vec3.fromValues(1.0, 0.0, 0.0);
const up    = glMatrix.vec3.fromValues(0.0, 1.0, 0.0);
const forward = glMatrix.vec3.fromValues(0.0, 0.0, 1.0);

function IntersectsTriangleAabbSat(v0, v1, v2, aabbExtents, axis) {
    const p0 = glMatrix.vec3.dot(v0, axis);
    const p1 = glMatrix.vec3.dot(v1, axis);
    const p2 = glMatrix.vec3.dot(v2, axis);

    const r =   aabbExtents[0] * Math.abs(glMatrix.vec3.dot(right, axis)) +
                aabbExtents[1] * Math.abs(glMatrix.vec3.dot(up, axis)) +
                aabbExtents[2] * Math.abs(glMatrix.vec3.dot(forward, axis));

    const maxP = Math.max(p0, Math.max(p1, p2));
    const minP = Math.min(p0, Math.min(p1, p2));

    return !(Math.max(-maxP, minP) > r);
}

// https://bronsonzgeb.com/index.php/2021/05/29/gpu-mesh-voxelizer-part-2/
function triangleAabb(A, B, C, aabb) {
    const center = aabb.center;
    const extents = aabb.extents;

    const a = glMatrix.vec3.clone(A);
    glMatrix.vec3.sub(a, a, center);

    const b = glMatrix.vec3.clone(B);
    glMatrix.vec3.sub(b, b, center);

    const c = glMatrix.vec3.clone(C);
    glMatrix.vec3.sub(c, c, center);

    const ab = glMatrix.vec3.create();
    glMatrix.vec3.sub(ab, b, a);
    glMatrix.vec3.normalize(ab, ab);

    const bc = glMatrix.vec3.create();
    glMatrix.vec3.sub(bc, c, b);
    glMatrix.vec3.normalize(bc, bc);

    const ca = glMatrix.vec3.create();
    glMatrix.vec3.sub(ca, a, c);
    glMatrix.vec3.normalize(ca, ca);

    //Cross ab, bc, and ca with (1, 0, 0)
    const a00 = glMatrix.vec3.fromValues(0.0, -ab[2], ab[1]);
    const a01 = glMatrix.vec3.fromValues(0.0, -bc[2], bc[1]);
    const a02 = glMatrix.vec3.fromValues(0.0, -ca[2], ca[1]);

    //Cross ab, bc, and ca with (0, 1, 0)
    const a10 = glMatrix.vec3.fromValues(ab[2], 0.0, -ab[0]);
    const a11 = glMatrix.vec3.fromValues(bc[2], 0.0, -bc[0]);
    const a12 = glMatrix.vec3.fromValues(ca[2], 0.0, -ca[0]);

    //Cross ab, bc, and ca with (0, 0, 1)
    const a20 = glMatrix.vec3.fromValues(-ab[1], ab[0], 0.0);
    const a21 = glMatrix.vec3.fromValues(-bc[1], bc[0], 0.0);
    const a22 = glMatrix.vec3.fromValues(-ca[1], ca[0], 0.0);

    const abxbc = glMatrix.vec3.create();
    glMatrix.vec3.cross(abxbc, ab, bc);

    if (
        !IntersectsTriangleAabbSat(a, b, c, extents, a00) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a01) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a02) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a10) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a11) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a12) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a20) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a21) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, a22) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, glMatrix.vec3.fromValues(1, 0, 0)) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, glMatrix.vec3.fromValues(0, 1, 0)) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, glMatrix.vec3.fromValues(0, 0, 1)) ||
        !IntersectsTriangleAabbSat(a, b, c, extents, abxbc)
    )
    {
        return false;
    }

    return true;
}

module.exports = {
    triangleAabb: triangleAabb
};