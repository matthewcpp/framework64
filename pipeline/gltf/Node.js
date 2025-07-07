const glMatrix = require("gl-matrix");

class N64Node {

    // this needs to correspond to fw64ColliderType in collider.h
    static ColliderType = {
        Box: 0,
        CollisionMesh: 1
    };

    static InvalidNodeIndex = 0xFFFFFFFF;

    static NoMesh = 0xFFFFFFFF;
    static MeshIgnored = 0xFFFFFFFE;
    static NoCollider = 0xFFFFFFFF;

    name;
    position = [0.0, 0.0, 0.0];
    rotation = [0.0, 0.0, 0.0, 1.0]
    scale = [1.0, 1.0, 1.0];

    localMatrix = glMatrix.mat4.create();
    worldMatrix = glMatrix.mat4.create();

    /* 
    this holds and array of child node objects
    this array is populated during level processing
    */
    childNodes = [];

    /* parent node object. root node will be null */
    parentNode = null;

    /* this will be filled in at the end of scene processing */
    firstChildNode = null;

    /* this will be filled in at the end of scene processing */
    nextSiblingNode = null;

    mesh = N64Node.NoMesh;
    collider = N64Node.NoCollider;
    data = 0;
    layerMask = 1;

    index = N64Node.InvalidNode;

    constructor(index, parentNode) {
        this.index = index;
        this.parentNode = parentNode;
    }


    computeWorldMatrix() {
        glMatrix.mat4.fromRotationTranslationScale(this.localMatrix, this.rotation, this.position, this.scale);

        if (this.parentNode === null) {
            glMatrix.mat4.copy(this.worldMatrix, this.localMatrix);
        } else {
            glMatrix.mat4.multiply(this.worldMatrix, this.parentNode.worldMatrix, this.localMatrix);
        }
    }
}

module.exports = N64Node;