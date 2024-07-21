
class N64Node {

    // this needs to correspond to fw64ColliderType in collider.h
    static ColliderType = {
        Box: 0,
        CollisionMesh: 1
    };

    static NoMesh = 0xFFFFFFFF;
    static MeshIgnored = 0xFFFFFFFE;
    static NoCollider = 0xFFFFFFFF;

    name;
    position = [0.0, 0.0, 0.0];
    rotation = [0.0, 0.0, 0.0, 1.0]
    scale = [1.0, 1.0, 1.0];
    mesh = N64Node.NoMesh;
    collider = N64Node.NoCollider;
    data = 0;
    layerMask = 1;
}

module.exports = N64Node;