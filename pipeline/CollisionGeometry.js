const Bounding = require("./gltf/Bounding");
const N64Node = require("./gltf/Node")
const glMatrix = require("gl-matrix");
const Intersect2d = require("./Intersect2d");

class BoundingRect {
    min = glMatrix.vec2.fromValues(Number.POSITIVE_INFINITY, Number.POSITIVE_INFINITY);
    max = glMatrix.vec2.fromValues(Number.NEGATIVE_INFINITY, Number.NEGATIVE_INFINITY);

    constructor(min, max) {
        if (typeof(min) !== "undefined" && typeof(max) !== "undefined") {
            glMatrix.vec2.copy(this.min, min);
            glMatrix.vec2.copy(this.max, max);
        }
    }
}

class CollisionGeometryCell {
    posX;
    posZ;
    boundingRect;
    walls = [];
    floors = [];
    ceilings = [];

    constructor(cellX, cellZ, boundingRect) {
        this.posX = cellX;
        this.posZ = cellZ;
        this.boundingRect = boundingRect;
    }

    get triangleCount () {
        return this.walls.length + this.floors.length + this.ceilings.length;
    }
}

class CollisionGeometry {
    cells = [];
    boundingBox;
    boundingRect;

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

    constructor(bounding, countX, countZ) {
        this.boundingBox = bounding;

        // set our bounding rect to the XZ bounding of the supplied AABB
        this.boundingRect = new BoundingRect(
            glMatrix.vec2.fromValues(bounding.min[0], bounding.min[2]),
            glMatrix.vec2.fromValues(bounding.max[0], bounding.max[2]));

        // partition the bounding into our grid
        this.cellCountX = countX;
        this.cellCountZ = countZ;

        // determine cell size
        this.cellSizeX = (this.boundingRect.max[0] - this.boundingRect.min[0]) / this.cellCountX;
        this.cellSizeZ = (this.boundingRect.max[1] - this.boundingRect.min[1]) / this.cellCountZ;

        const cellMinPos = glMatrix.vec2.clone(this.boundingRect.min);
        const cellMaxPos = glMatrix.vec2.create();

        for (let z = 0; z < this.cellCountZ; z++) {
            for (let x = 0; x < this.cellCountX; x++) {
                glMatrix.vec2.set(cellMaxPos, cellMinPos[0] + this.cellSizeX, cellMinPos[1] + this.cellSizeZ);
                const cellBounding = new BoundingRect(cellMinPos, cellMaxPos);
                this.cells.push(new CollisionGeometryCell(x, z, cellBounding));
            }

            // update the min pos for the next cell
            glMatrix.vec2.set(cellMinPos, this.boundingRect.min[0], cellMinPos[1] + this.cellSizeZ);
        }
    }

    get triangleCount() {
        let total = 0;

        for (const cell of this.cells) {
            total += cell.triangleCount;
        }

        return total;
    }

    getOverlappingCells(triangle) {
        // create the 2d xz triangle
        const a = glMatrix.vec2.fromValues(triangle[0][0], triangle[0][2]);
        const b = glMatrix.vec2.fromValues(triangle[1][0], triangle[1][2]);
        const c = glMatrix.vec2.fromValues(triangle[2][0], triangle[2][2]);

        // create a bounding rectangle
        const min = glMatrix.vec2.clone(a);
        const max = glMatrix.vec2.clone(b);

        glMatrix.vec2.min(min, min, b);
        glMatrix.vec2.max(max, max, b);
        glMatrix.vec2.min(min, min, c);
        glMatrix.vec2.max(max, max, c);

        // determine the range of cells to query.
        // Precondition: all triangles fit in the grid.
        const minCellX = Math.floor((min[0] - this.boundingRect.min[0]) / this.cellSizeX);
        const maxCellX = Math.ceil((max[0] - this.boundingRect.min[0]) / this.cellSizeX);

        const minCellZ = Math.floor((min[1] - this.boundingRect.min[1]) / this.cellSizeZ);
        const maxCellZ = Math.ceil((max[1] - this.boundingRect.min[1]) / this.cellSizeZ);

        const overlappingCells = [];
        for (let z = minCellZ; z < maxCellZ; z++) {
            for (let x = minCellX; x < maxCellX; x++) {
                const cellIndex = z * this.cellCountX + x;
                const cell = this.cells[cellIndex];
                if (Intersect2d.triangleAABB(a, b, c, cell.boundingRect.min, cell.boundingRect.max)) {
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