const Bounding = require("./gltf/Bounding");
const N64Node = require("./gltf/Node")
const glMatrix = require("gl-matrix");
const Intersect = require("./Intersect");

class CollisionGeometryCell {
    posX;
    posZ;
    boundingBox;
    walls = [];
    floors = [];
    ceilings = [];

    constructor(cellX, cellZ, boundingBox) {
        this.posX = cellX;
        this.posZ = cellZ;
        this.boundingBox = boundingBox;
    }

    get triangleCount () {
        return this.walls.length + this.floors.length + this.ceilings.length;
    }
}

class CollisionGeometry {
    cells = [];
    boundingBox;

    cellCountX;
    cellCountZ;

    cellSizeX;
    cellSizeZ;

    numUniqueTriangles = 0;
    numSingleCellTriangles = 0;
    numMultiCellTriangles = 0;

    static _up = glMatrix.vec3.fromValues(0, 1, 0);

    /** In order to be a floor, the Y component of a triangle's normal must be greater than this value
     *  In order to be a ceiling, the Y component of a triangle's normal must be less than -1 * this value
    */
    floorAndCeilingTolerance = 0.05;

    constructor(sceneBounding, countX, countZ) {
        this.boundingBox = sceneBounding;

        // partition the bounding into a uniform grid
        this.cellCountX = countX;
        this.cellCountZ = countZ;

        // determine cell size
        this.cellSizeX = (this.boundingBox.max[0] - this.boundingBox.min[0]) / this.cellCountX;
        this.cellSizeZ = (this.boundingBox.max[2] - this.boundingBox.min[2]) / this.cellCountZ;

        const cellMinPos = glMatrix.vec3.clone(this.boundingBox.min);
        const cellMaxPos = glMatrix.vec3.create();

        for (let z = 0; z < this.cellCountZ; z++) {
            for (let x = 0; x < this.cellCountX; x++) {
                glMatrix.vec3.set(cellMaxPos, cellMinPos[0] + this.cellSizeX, this.boundingBox.max[1], cellMinPos[2] + this.cellSizeZ);
                const cellBounding = Bounding.createFromMinMax(cellMinPos, cellMaxPos);
                this.cells.push(new CollisionGeometryCell(x, z, cellBounding));
            }

            // update the min pos for the next cell
            glMatrix.vec3.set(cellMinPos, this.boundingBox.min[0], this.boundingBox.min[1], cellMinPos[2] + this.cellSizeZ);
        }
    }

    get triangleCount() {
        let total = 0;

        for (const cell of this.cells) {
            total += cell.triangleCount;
        }

        return total;
    }

    /** The basic approach we will take here is to create a bounding box for the triangle and then check all the
     *  cells that intersect that box.
     *  TODO: investigate a more efficient approach such as the one outlined here: https://www.jb101.co.uk/2008/08/09/partitioning-triangles-into-a-uniform-grid.html
     */
    getOverlappingCells(triangle) {
        // create a bounding rectangle
        const A = triangle[0];
        const B = triangle[1];
        const C = triangle[2];

        const bounding = new Bounding();
        bounding.encapsulatePoint(A);
        bounding.encapsulatePoint(B);
        bounding.encapsulatePoint(C);

        // determine the range of cells to query.
        // Precondition: all triangles fit in the grid.
        const minCellX = Math.floor((bounding.min[0] - this.boundingBox.min[0]) / this.cellSizeX);
        const maxCellX = Math.ceil((bounding.max[0] - this.boundingBox.min[0]) / this.cellSizeX);

        const minCellZ = Math.floor((bounding.min[2] - this.boundingBox.min[2]) / this.cellSizeZ);
        const maxCellZ = Math.ceil((bounding.max[2] - this.boundingBox.min[2]) / this.cellSizeZ);

        const overlappingCells = [];
        for (let z = minCellZ; z < maxCellZ; z++) {
            for (let x = minCellX; x < maxCellX; x++) {
                const cellIndex = z * this.cellCountX + x;
                const cell = this.cells[cellIndex];
                if (Intersect.triangleAabb(A, B, C, cell.boundingBox)) {
                    overlappingCells.push(cell);
                }
            }
        }

        if (overlappingCells.length > 1) {
            this.numMultiCellTriangles +=1;
        } else {
            this.numSingleCellTriangles += 1;
        }

        return overlappingCells;
    }

    insertTriangle(triangle) {
        const cells = this.getOverlappingCells(triangle);
        const normal = triangle[3];

        if (normal[1] > this.floorAndCeilingTolerance) {
            for (const cell of cells) {
                cell.floors.push(triangle);
            }
        } else if (normal[1] < -this.floorAndCeilingTolerance) {
            for (const cell of cells) {
                cell.ceilings.push(triangle);
            }
        } else {
            for (const cell of cells) {
                cell.walls.push(triangle);
            }
        }

        this.numUniqueTriangles += 1;
    }

    static createFromScene(scene, gltfData) {
        // first determine nodes we will need to process and compute the scene's world space bounding box
        const worldGeometryNodes = [];
        const sceneBounding = new Bounding();
        for (const node of scene.nodes) {
            // we can filter out nodes which do not need static geometry built.
            if (node.mesh === N64Node.NoMesh || node.collider == N64Node.NoCollider || node.collisionType == N64Node.CollisionType.Dynamic) {
                continue;
            }
            const mesh = gltfData.meshes[node.mesh];
            const meshBounding = mesh.bounding;
            const worldBounding = Bounding.transformByMatrix(meshBounding, node.worldMatrix);
            sceneBounding.encapsulateBox(worldBounding);

            worldGeometryNodes.push(node);
        }

        const geometry = new CollisionGeometry(sceneBounding, 1, 3);

        const AB = glMatrix.vec3.create();
        const AC = glMatrix.vec3.create();

        for (const node of worldGeometryNodes) {
            const mesh = gltfData.meshes[node.mesh];

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