const Bounding = require("./gltf/Bounding");
const N64Node = require("./gltf/Node")
const GLTFUtil = require("./gltf/GLTFUtil");
const glMatrix = require("gl-matrix");
const Intersect = require("./Intersect");

/** Represents a bounding volume around a group of triangles in a single grid cell */
class CollisionGeometryBoundingVolume {
    static Type = {
        Box: 1
    }

    node;
    type = CollisionGeometryBoundingVolume.Box;

    /** currently this is assumed to be a bounding box.  it may support more primitives in the future */
    volume = new Bounding();
    floors = [];
    walls = [];
    ceilings = [];

    constructor(node) {
        this.node = node;
    }

    get triangleCount () {
        return this.walls.length + this.floors.length + this.ceilings.length;
    }
}

class CollisionGeometryCell {
    posX;
    posZ;
    boundingBox;
    ladders = [];
    boundingVolumes = [];

    constructor(cellX, cellZ, boundingBox) {
        this.posX = cellX;
        this.posZ = cellZ;
        this.boundingBox = boundingBox;
    }

    /** 
     * Note: This implementation assumes that all of the triangles for a node are added before any from the next node.
     * If that were to change then we would probably need to move to a map or something along those lines. */
    _getOrCreateBoundingVolumeForNode(node) {
        if (this.boundingVolumes.length > 0) {
            const boundingVolume = this.boundingVolumes[this.boundingVolumes.length - 1];

            if (boundingVolume.node.index == node.index) {
                return boundingVolume;
            }
        }

        const boundingVolume = new CollisionGeometryBoundingVolume(node);
        this.boundingVolumes.push(boundingVolume);

        return boundingVolume;
    }

    static _addTriangleToBoundingVolume(boundingVolume, triangle) {
        boundingVolume.volume.encapsulatePoint(triangle[0]);
        boundingVolume.volume.encapsulatePoint(triangle[1]);
        boundingVolume.volume.encapsulatePoint(triangle[2]);
    }

    addFloor(node, triangle) {
        const boundingVolume = this._getOrCreateBoundingVolumeForNode(node);
        CollisionGeometryCell._addTriangleToBoundingVolume(boundingVolume, triangle);
        boundingVolume.floors.push(triangle);
    }

    addWall(node, triangle) {
        const boundingVolume = this._getOrCreateBoundingVolumeForNode(node);
        CollisionGeometryCell._addTriangleToBoundingVolume(boundingVolume, triangle);
        boundingVolume.walls.push(triangle);
    }

    addCeiling(node, triangle) {
        const boundingVolume = this._getOrCreateBoundingVolumeForNode(node);
        CollisionGeometryCell._addTriangleToBoundingVolume(boundingVolume, triangle);
        boundingVolume.ceilings.push(triangle);
    }
}

class CollisionGeometryLadder {
    entrance;
    exit;
    normal;
    radius;

    constructor(entrance, exit, normal, radius) {
        this.entrance = entrance;
        this.exit = exit;
        this.normal = normal;
        this.radius = radius;
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
        this.cellCountY = 1; // grid is flat...for now
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
            for (const boundingVolume of cell.boundingVolumes) {
                total += boundingVolume.triangleCount;
            }
        }

        return total;
    }

    get ladderCount() {
        let total = 0;

        for (const cell of this.cells) {
            total += cell.ladders.length;
        }

        return total;
    }

    get boundingVolumeCount() {
        let total = 0;

        for (const cell of this.cells) {
            total += cell.boundingVolumes.length;
        }

        return total;
    }

    _getOverlappingCells(bounding) {
        // determine the range of cells to query.
        const minCellX = Math.floor((bounding.min[0] - this.boundingBox.min[0]) / this.cellSizeX);
        const maxCellX = Math.min(Math.ceil((bounding.max[0] - this.boundingBox.min[0]) / this.cellSizeX), this.cellCountX - 1);

        const minCellZ = Math.floor((bounding.min[2] - this.boundingBox.min[2]) / this.cellSizeZ);
        const maxCellZ = Math.min(Math.ceil((bounding.max[2] - this.boundingBox.min[2]) / this.cellSizeZ), this.cellCountZ - 1);

        // note about iteration below: we want to make sure we inlucle all cells in the min/max range so we use <=
        const overlappingCells = [];
        for (let z = minCellZ; z <= maxCellZ; z++) {
            for (let x = minCellX; x <= maxCellX; x++) {
                const cellIndex = z * this.cellCountX + x;
                overlappingCells.push(this.cells[cellIndex]);
            }
        }

        return overlappingCells;
    }

    /** The basic approach we will take here is to create a bounding box for the triangle and then check all the
     *  cells that intersect that box.
     *  TODO: investigate a more efficient approach such as the one outlined here: https://www.jb101.co.uk/2008/08/09/partitioning-triangles-into-a-uniform-grid.html
     */
    _getGridCellsForTriangle(triangle) {
        // create a bounding rectangle
        const A = triangle[0];
        const B = triangle[1];
        const C = triangle[2];

        const bounding = new Bounding();
        bounding.encapsulatePoint(A);
        bounding.encapsulatePoint(B);
        bounding.encapsulatePoint(C);

        const overlappingCells = this._getOverlappingCells(bounding).filter((cell) => {
            return Intersect.triangleAabb(A, B, C, cell.boundingBox);
        });

        if (overlappingCells.length > 1) {
            this.numMultiCellTriangles +=1;
        } else {
            this.numSingleCellTriangles += 1;
        }

        return overlappingCells;
    }

    insertTriangle(node, triangle) {
        const cells = this._getGridCellsForTriangle(triangle);
        const normal = triangle[3];

        const minY = Math.min(triangle[0][1], triangle[1][1], triangle[2][1]);
        const maxY = Math.max(triangle[0][1], triangle[1][1], triangle[2][1]);
        triangle.push(minY);
        triangle.push(maxY);

        if (normal[1] > this.floorAndCeilingTolerance) {
            for (const cell of cells) {
                cell.addFloor(node, triangle);
            }
        } else if (normal[1] < -this.floorAndCeilingTolerance) {
            for (const cell of cells) {
                cell.addCeiling(node, triangle);
            }
        } else {
            for (const cell of cells) {
                cell.addWall(node, triangle);
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
                    geometry.insertTriangle(node, triangle);
                }
            }
        }

        if (degenerateCount > 0) {
            console.log(`Warning: ${degenerateCount} degenerate trigangles filtered when creating collision geometry in scene: ${scene.name}.`);
        }

        CollisionGeometry._processLadders(geometry, scene, gltfData);

        return geometry;
    }

    insertLadder(entrance, exit, normal, radius) {
        const ladder = new CollisionGeometryLadder(entrance, exit, normal, radius);

        const bounding = new Bounding();
        bounding.encapsulatePoint(entrance);
        bounding.encapsulatePoint(exit);

        for (const cell of this._getOverlappingCells(bounding)) {
            cell.ladders.push(ladder);
        }
    }

    static _processLadders(geometry, scene, gltfData) {
        if (!scene.nodeTypes.has("ladder")) {
            return;
        }

        const ladderNodes = scene.nodeTypes.get("ladder");

        const ladderNormalMatrix = glMatrix.mat3.create();
        const forward = glMatrix.vec3.fromValues(0.0, 0.0, -1.0);

        for (const ladderNode of ladderNodes) {
            // get the entrance and exit position of the ladder
            const entranceNode = GLTFUtil.findChildNodeStartingWith(gltfData, ladderNode.gltfNode, "_entrance");
            const exitNode = GLTFUtil.findChildNodeStartingWith(gltfData, ladderNode.gltfNode, "_exit");

            if (entranceNode === null || exitNode === null) {
                throw new Error(`Ladder Node: ${ladderNode.name} must have child nodes '_entrance' and '_exit'`);
            }

            if (!Object.hasOwn(ladderNode.gltfNode.extras, "ladderRadius")) {
                throw new Error(`Ladder Node: ${ladderNode.name} must have a 'ladderRadius' custom property.`);
            }

            const radius = parseFloat(ladderNode.gltfNode.extras.ladderRadius);

            const entrancePos = glMatrix.vec3.create();
            glMatrix.vec3.transformMat4(entrancePos, entranceNode.translation, ladderNode.worldMatrix);

            const exitPos = glMatrix.vec3.create();
            glMatrix.vec3.transformMat4(exitPos, exitNode.translation, ladderNode.worldMatrix);

            const normal = glMatrix.vec3.create();
            
            glMatrix.mat3.fromMat4(ladderNormalMatrix, ladderNode.worldMatrix);
            glMatrix.vec3.transformMat3(normal, forward, ladderNormalMatrix);
            glMatrix.vec3.normalize(normal, normal);

            geometry.insertLadder(entrancePos, exitPos, normal, radius);
        }
    }
}

module.exports = CollisionGeometry;