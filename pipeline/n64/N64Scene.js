class N64Scene {
    name = "scene";

    /** this is the root GLTF node representing the scene. */
    rootNode = null;
    nodes = [];
    meshes = [];
    colliderCount = 0;
    collisionMeshes = [];
    customBoundingBoxes = [];
    meshResources = null;
}

module.exports = N64Scene;