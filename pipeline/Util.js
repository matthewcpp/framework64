const path = require("path");

//https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
function nextPowerOf2(v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return ++v;
}

function isPowerOf2(x) {
    return (x & (x - 1)) === 0;
}

function safeDefineName(name) {
    return name.replaceAll(' ', '_').replaceAll('-', '_').replaceAll('.', '_');
}

function mat4IdentityBuffer() {
    const b = Buffer.alloc(16 * 4, 0);
    b.writeFloatLE(1.0, 0);
    b.writeFloatLE(1.0, 5 * 4);
    b.writeFloatLE(1.0, 10 * 4);
    b.writeFloatLE(1.0, 15 * 4);
    return b;
}

/** Returns the asset include directory given the base asset output directory. */
function assetIncludeDirectory(outputDirectory) {
    return path.join(outputDirectory, "include", "assets");
}

module.exports = {
    nextPowerOf2: nextPowerOf2,
    isPowerOf2: isPowerOf2,
    safeDefineName: safeDefineName,
    mat4IdentityBuffer: mat4IdentityBuffer,
    assetIncludeDirectory: assetIncludeDirectory
}
