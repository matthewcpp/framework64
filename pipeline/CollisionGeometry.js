const Bounding = require("./gltf/Bounding");
const N64Node = require("./gltf/Node")
const glMatrix = require("gl-matrix");
const Intersect = require("./Intersect");

class CollisionGeometryNode {
    id;
    walls = [];
    floors = [];
    ceilings = [];

    constructor(id) {
        this.id = id;
    }
}

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
                cellMinPos[0] += this.cellSizeX;
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
        if (scene.gridSize === null) {
            throw new Error(`Could not create collision geometry for ${scene.name}: grid size not specified`)
        }

        // first determine nodes we will need to process and compute the scene's world space bounding box
        const worldGeometryNodes = [];
        const sceneBounding = new Bounding();
        let degenerateCount = 0;
        for (const node of scene.nodes) {
            // we can filter out nodes which do not need static geometry built.
            if (node.mesh === N64Node.NoMesh || node.collider == N64Node.NoCollider || node.collisionType == N64Node.CollisionType.Dynamic) {
                continue;
            }

            const mesh = gltfData.meshes[scene.meshBundle[node.mesh]];
            const meshBounding = mesh.bounding;
            const worldBounding = Bounding.transformByMatrix(meshBounding, node.worldMatrix);
            sceneBounding.encapsulateBox(worldBounding);

            worldGeometryNodes.push(node);
        }

        const geometry = new CollisionGeometry(sceneBounding, scene.gridSize[0], scene.gridSize[1]);


        const AB = glMatrix.vec3.create();
        const AC = glMatrix.vec3.create();

        // It is possible that degenrate triangles may result in a divide by zero exception at runtime.
        // This likely occurs due to the rounding of vertices that occurs when converting to N64 vertex format
        // We attempt to guard against this by filtering out these triangles here.
        // TODO: look into detecting / fixing this at a higher level?
        const _triangleIsDegenerate = (a, b, c) => {
            return glMatrix.vec3.exactEquals(a,b) || glMatrix.vec3.exactEquals(b,c) || glMatrix.vec3.exactEquals(a,c);
        }

        for (const node of worldGeometryNodes) {
            const mesh = gltfData.meshes[scene.meshBundle[node.mesh]];

            for (const primitive of mesh.primitives) {
                for (const element of primitive.elements) {
                    const a = primitive.vertices[element[0]];
                    const b = primitive.vertices[element[1]]
                    const c = primitive.vertices[element[2]]

                    const A = glMatrix.vec3.create();
                    const B = glMatrix.vec3.create();
                    const C = glMatrix.vec3.create();
                    const N = glMatrix.vec3.create();

                    if (_triangleIsDegenerate(a, b, c)) {
                        degenerateCount += 1;
                        continue;
                    }

                    glMatrix.vec3.transformMat4(A, a, node.worldMatrix);
                    glMatrix.vec3.transformMat4(B, b, node.worldMatrix);
                    glMatrix.vec3.transformMat4(C, c, node.worldMatrix);
                    
                    glMatrix.vec3.subtract(AB, B, A);
                    glMatrix.vec3.subtract(AC, C, A);

                    glMatrix.vec3.cross(N, AB, AC);
                    glMatrix.vec3.normalize(N, N);

                    const triangle = [A, B, C, N];
                    geometry.insertTriangle(triangle);
                }
            }
        }

        if (degenerateCount > 0) {
            console.log(`Warning: ${degenerateCount} degenerate trigangles filtered when creating collision geometry in scene: ${scene.name}.`);
        }

        return geometry;
    }
}

module.exports = CollisionGeometry;