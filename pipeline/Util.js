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
    return name.replaceAll(' ', '_').replaceAll('-', '_');
}

module.exports = {
    nextPowerOf2: nextPowerOf2,
    isPowerOf2: isPowerOf2,
    safeDefineName: safeDefineName
}
