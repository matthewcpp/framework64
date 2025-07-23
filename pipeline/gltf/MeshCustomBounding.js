const glMatrix = require("gl-matrix");
const Bounding = require("./Bounding");

/** Custom bounding box for a mesh can be stored in a node named _boundingBox
 *  In the event sucha. node is found, a box is set via center (position) and extents (scale)
 */
function setStaticMeshCustomBoundingBox(mesh, gltfLoader) {
    for (const gltfNode of gltfLoader.gltf.nodes) {
        if (gltfNode.name == "_boundingBox") {
            const center = glMatrix.vec3.fromValues(0.0, 0.0, 0.0);
            const extents = glMatrix.vec3.fromValues(1.0, 1.0, 1.0);

            if (gltfNode.translation) {
                glMatrix.vec3.copy(center, gltfNode.translation);
            }

            if (gltfNode.scale) {
                glMatrix.vec3.copy(extents, gltfNode.scale);
            }

            mesh.customBoundingBox = Bounding.createFromCenterAndExtents(center, extents);
            return;
        }
    }
}

module.exports = {
    setForStaticMesh: setStaticMeshCustomBoundingBox
};
