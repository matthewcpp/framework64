const glMatrix = require("gl-matrix");

class N64Node {

    // this needs to correspond to fw64ColliderType in collider.h
    static ColliderType = {
        Box: 0,
        Sphere: 1,
        CollisionMesh: 2
    };

    static CollisionType = {
        /** stationary world geometry */
        Static: 0,

        /** synamic, movable geometry */
        Dynamic: 1
    };

    static InvalidNodeIndex = 0xFFFFFFFF;

    static NoMesh = 0xFFFFFFFF;
    /** Temporary value for the mesh field to indicate to the parse that the mesh for this node was explicity skipped
     *  and any value present in the gltf should not be processed.
     */
    static MeshIgnored = 0xFFFFFFFE;

    static UnspecifiedCollider = 0xFFFFFFFE;
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

    /** This holds the index into the parsed gltf data's mesh array */
    mesh = N64Node.NoMesh;
    collider = N64Node.UnspecifiedCollider;
    collisionType = N64Node.CollisionType.Static;
    data = 0;
    layerMask = 1;

    /** This is the node's own index in the scene's node array */
    index = N64Node.InvalidNode;

    /** pointer to the source gltf node structure */
    gltfNode;

    /** The specific type of this node.  eg. "ladder"
     * The default value is null, indicating this node is not special.
     * This value is not specifially used by the level parser itself, but is checked by collision geometry builder.
     */
    nodeType = null;

    constructor(index, parentNode, gltfNode) {
        this.index = index;
        this.parentNode = parentNode;
        this.gltfNode = gltfNode;
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