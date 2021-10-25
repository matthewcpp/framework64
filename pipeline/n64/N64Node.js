


class N64Node {

    static NoMesh = 0xFFFFFFFF;

    position = [0.0, 0.0, 0.0];
    rotation = [0.0, 0.0, 0.0, 1.0]
    scale = [1.0, 1.0, 1.0];
    mesh = N64Node.NoMesh;
    collider = 0;
    type = 0;
    layerMask = 0;
}

module.exports = N64Node;