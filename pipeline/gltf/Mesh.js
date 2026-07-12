const Bounding = require("./Bounding");
const Splitter = require("./Splitter");

class Mesh {
    name;
    primitives = [];
    materialBundle = null;

    /** 
     * this custom bounding box is currently optionally set when processing static and skinned meshes
     * TODO: set this when processing meshes in levels too. */
    customBoundingBox = null;

    /** this is the computed bounding box based solely on the points in the mesh 
     * lazily computed by calling .pointBoundingBox
    */
    _boundingBox = null;

    constructor(name) {
        this.name = name;
    }

    /** gets the effective bounding of the mesh */
    get bounding() {
        if (this.customBoundingBox !== null) {
            return this.customBoundingBox;
        }

        return this.pointBoundingBox;
    }

    /** lazily computes the bounding based on the mesh's points (if necessary) */
    get pointBoundingBox() {
        if (this._boundingBox !== null) {
            return this._boundingBox;
        }

        this._boundingBox = new Bounding();

        for (const primitive of this.primitives) {
            this._boundingBox.encapsulateBox(primitive.bounding);
        }

        return this._boundingBox;
    }

    get hasNormals() {
        for (const mesh of this.primitives) {
            if (mesh.hasNormals)
                return true;
        }
        return false;
    }

    get hasMaterialBundle() {
        return this.materialBundle != null;
    }

    get triangleCount() {
        let count = 0;
        for (const primitive of this.primitives) {
            count += primitive.elements.length;
        }

        return count;
    }

    get isSkinned() {
        if (this.primitives.length === 0) {
            return false;
        } else {
            return this.primitives[0].jointIndices != null;
        }
    }

    // called by the GLTF loader
    prunePrimitiveVertices() {
        for (const primitive of this.primitives) {
            primitive.pruneUnusedVertices();
        }
    }

    // due to the way GLTF is exported from blender, it is possible that primitives may
    // contain faces which are influenced by more than one bone.
    // In this case we need to split the primitives so that each draw call uses the correct joint matrix
    // This should be called when processing a skinned mesh
    splitPrimitivesForSkinning(){
        const primitives = this.primitives;
        this.primitives = [];

        for (const primitive of primitives) {
            if (primitive.containsMultipleJointIndices) {
                this.primitives.push(...Splitter.split(primitive));
            }
            else {
                this.primitives.push(primitive);
            }
        }
    }

    sortPrimitives(materials) {
        this.primitives.sort((primA, primB) => {
            const matA = materials[primA.material];
            const matB = materials[primB.material];

            if (matA.shadingMode < matB.shadingMode) {
                return -1;
            } else if (matA.shadingMode == matB.shadingMode) {
                return 0;
            } else {
                return 1;
            }
        });
    }

    remapJointIndices(jointMap) {
        for (const primitive of this.primitives) {
            if (!jointMap.has(primitive.jointIndices[0])) {
                throw new Error("Unable map to all animation joints");
            }

            const jointIndex = jointMap.get(primitive.jointIndices[0]);

            primitive.jointIndices[0] = jointIndex;
        }
    }
}

module.exports = Mesh;
