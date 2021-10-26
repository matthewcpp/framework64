
class N64Node {

    // this needs to correspond to fw64ColliderType in collider.h
    static ColliderType = {
        None: 0,
        Box: 1,
        Mesh: 2
    };

    static NoMesh = 0xFFFFFFFF;
    static NoCollider = 0xFFFFFFFF;
    static NoType = 0xFFFFFFFF

    position = [0.0, 0.0, 0.0];
    rotation = [0.0, 0.0, 0.0, 1.0]
    scale = [1.0, 1.0, 1.0];
    mesh = N64Node.NoMesh;
    collider = N64Node.NoCollider;
    type = N64Node.NoType;
    layerMask = 1;
}

module.exports = N64Node;