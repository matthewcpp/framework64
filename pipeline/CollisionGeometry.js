const Bounding = require("./gltf/Bounding");
const N64Node = require("./gltf/Node")
const glMatrix = require("gl-matrix");

class CollisionGeometryCell {
    walls = [];
    floors = [];
    ceilings = [];

    get triangleCount () {
        return this.walls.length + this.floors.length + this.ceilings.length;
    }
}

class CollisionGeometry {
    cells = [];
    boundingBox = new Bounding();
    cellCountX;
    cellCountZ;

    static _up = glMatrix.vec3.fromValues(0, 1, 0);

    /** In order to be a floor, the Y component of a triangle's normal must be greater than this value
     *  In order to be a ceiling, the Y component of a triangle's normal must be less than -1 * this value
    */
    floorAndCeilingTolerance = 0.9;


    constructor(countX, countZ) {
        this.cellCountX = countX;
        this.cellCountZ = countZ;

        for (let i = 0; i < this.cellCountX * this.cellCountZ; i++) {
            this.cells.push(new CollisionGeometryCell());
        }
    }

    get triangleCount() {
        let total = 0;

        for (const cell of this.cells) {
            total += cell.triangleCount;
        }

        return total;
    }

    insertTriangle(triangle) {
        // TODO: find all cells this triangle would fit in
        // https://jtsorlinis.github.io/rendering-tutorial/
        // https://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
        // https://www.jb101.co.uk/2008/08/09/partitioning-triangles-into-a-uniform-grid.html
        const cell = this.cells[0];

        const upDot = glMatrix.vec3.dot(triangle[3], CollisionGeometry._up);

        if (upDot > this.floorAndCeilingTolerance) {
            cell.floors.push(triangle);
        } else if (upDot < -this.floorAndCeilingTolerance) {
            cell.ceilings.push(triangle);
        } else {
            cell.walls.push(triangle);
        }
    }

    static createFromScene(scene, gltfData) {
        const geometry = new CollisionGeometry(1, 1);

        const AB = glMatrix.vec3.create();
        const AC = glMatrix.vec3.create();

        for (const node of scene.nodes) {
            // we can filter out nodes which do not need static geometry built.
            if (node.mesh === N64Node.NoMesh || node.collider == N64Node.NoCollider || node.collisionType == N64Node.CollisionType.Dynamic) {
                continue;
            }

            const mesh = gltfData.meshes[node.mesh];
            const meshBounding = mesh.bounding;
            const worldBounding = Bounding.transformByMatrix(meshBounding, node.worldMatrix);
            geometry.boundingBox.encapsulateBox(worldBounding);
            
            for (const primitive of mesh.primitives) {
                for (const element of primitive.elements) {
                    const A = glMatrix.vec3.create();
                    const B = glMatrix.vec3.create();
                    const C = glMatrix.vec3.create();
                    const N = glMatrix.vec3.create();

                    glMatrix.vec3.transformMat4(A, primitive.vertices[element[0]], node.worldMatrix);
                    glMatrix.vec3.transformMat4(B, primitive.vertices[element[1]], node.worldMatrix);
                    glMatrix.vec3.transformMat4(C, primitive.vertices[element[2]], node.worldMatrix);
                    
                    glMatrix.vec3.subtract(AB, B, A);
                    glMatrix.vec3.subtract(AC, C, A);

                    glMatrix.vec3.cross(N, AB, AC);
                    glMatrix.vec3.normalize(N, N);

                    const triangle = [A, B, C, N];
                    geometry.insertTriangle(triangle);
                }
            }
        }

        return geometry;
    }
}

module.exports = CollisionGeometry;