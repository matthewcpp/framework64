const fs = require("fs");
const CollisionGeometry = require("../gltf/CollisionGeometry");
const Bounding = require("../gltf/Bounding");

class CollisionGeometryWriter {
    /** This needs to be kept in sync with fw64CollisionGeometryInfo in collision_geometry.h */
    static headerSize = ((3 /*element counts */ + 3 /* cell counts */) * 4)  + Bounding.SizeOf; /* bounding min/max */

    /** This needs to be kept in sync with fw64CollisionTriangle in collision_geometry.h */
    static triangleSize = (((3 * 3) /* points */ + (1 * 3) /* normal */) * 4) + Bounding.SizeOf;

    /** This needs to be kept in sync with fw64CollisionLadder in collision_geometry.h */
    static ladderSize = ((3 * 2) /* points */ + 3 /* normal */ + 1 /* radius */) * 4

    /** This needs to be kept in sync with fw64CollisionGeometryCell in collision_geometry.h */
    static cellSize = (2 /*ladder info */ + 2 /* bounding volume info */) * 2;

    static boundingVolumeSize = (8 * 2) + Bounding.SizeOf;

    /**offset (in bytes) into the binary triangle buffer used for writing */
    triangleBufferIndex = 0;

    /** fw64CollisionTriangle index offset into the triangle array. */
    triangleArrayIndex = 0;

    /** offset (in bytes) into the binary cell data used for writing */
    cellBufferIndex = 0;

    ladderArrayIndex = 0;

    /** offset (in bytes) into the binary ladder data used for writing */
    ladderBufferIndex = 0;

    /** fw64CollisionGeometryBoundingVolume index offset into the triangle array. */
    boundingVolumesArrayIndex = 0;

    /** offset (in bytes) into the binary bounding volumes data used for writing */
    boundingVolumesBufferIndex = 0;

    writer;

    cellBuffer;
    tringleBuffer;
    ladderBuffer;
    boundingVolumeBuffer;

    constructor(writer) {
        this.writer = writer;
    }

    writeToFile(collisionGeometry, filePath) {
        const file = fs.openSync(filePath);
        writeData(collisionGeometry, file);
        fs.closeSync(file);
    }

    writeData(collisionGeometry, file) {
        this.triangleBufferIndex = 0;
        this.triangleArrayIndex = 0;
        this.cellBufferIndex = 0;

        const triangleCount = collisionGeometry.triangleCount;
        const ladderCount = collisionGeometry.ladderCount;
        const boundingVolumeCount = collisionGeometry.boundingVolumeCount;

        // this needs to line up with fw64CollisionGeometryInfo in collision_geometry.c
        const headerBuffer = Buffer.alloc(CollisionGeometryWriter.headerSize);
        let headerIndex = 0;
        headerIndex = this.writer.writeUInt32(headerBuffer, triangleCount, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, ladderCount, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, boundingVolumeCount, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, collisionGeometry.cellCountX, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, collisionGeometry.cellCountY, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, collisionGeometry.cellCountZ, headerIndex);
        headerIndex = collisionGeometry.boundingBox.write(this.writer, headerBuffer, headerIndex);

        this.triangleBuffer = Buffer.alloc(CollisionGeometryWriter.triangleSize * triangleCount);
        this.cellBuffer = Buffer.alloc(CollisionGeometryWriter.cellSize * collisionGeometry.cells.length);
        this.ladderBuffer = Buffer.alloc(CollisionGeometryWriter.ladderSize * ladderCount);
        this.boundingVolumeBuffer = Buffer.alloc(CollisionGeometryWriter.boundingVolumeSize * boundingVolumeCount);

        for (const cell of collisionGeometry.cells) {
            this._writeCellToBuffer(cell);
        }

        fs.writeSync(file, headerBuffer);
        fs.writeSync(file, this.triangleBuffer);
        fs.writeSync(file, this.ladderBuffer);
        fs.writeSync(file, this.boundingVolumeBuffer);
        fs.writeSync(file, this.cellBuffer);
    }

    _writeCellToBuffer(cell) {
        this.cellBufferIndex = this.writer.writeUInt16(this.cellBuffer, this.ladderArrayIndex, this.cellBufferIndex);
        this.cellBufferIndex = this.writer.writeUInt16(this.cellBuffer, cell.ladders.length, this.cellBufferIndex);

        this.cellBufferIndex = this.writer.writeUInt16(this.cellBuffer, this.boundingVolumesArrayIndex, this.cellBufferIndex);
        this.cellBufferIndex = this.writer.writeUInt16(this.cellBuffer, cell.boundingVolumes.length, this.cellBufferIndex);

        this._writeLaddersToBuffer(cell.ladders);
        this._writeBoundingVolumesToBuffer(cell.boundingVolumes);
    }

    /** for now each triangle is array of length 4 contianing 4 vectors */
    _writeTrianglesToBuffer(triangles) {
        for (const triangle of triangles) {
            // write out points and vector
            for (let i = 0; i < 4; i++) {
                for (let j = 0; j < 3; j++) {
                    this.triangleBufferIndex = this.writer.writeFloat(this.triangleBuffer, triangle[i][j], this.triangleBufferIndex);
                }
            }

            // write the bounding box
            this.triangleBufferIndex = triangle[4].write(this.writer, this.triangleBuffer, this.triangleBufferIndex);
        }

        this.triangleArrayIndex += triangles.length;
    }

    _writeLaddersToBuffer(ladders){
        for (const ladder of ladders) {
            for (let i = 0; i < 3; i++) {
                this.ladderBufferIndex = this.writer.writeFloat(this.ladderBuffer, ladder.entrance[i], this.ladderBufferIndex);
            }

            for (let i = 0; i < 3; i++) {
                this.ladderBufferIndex = this.writer.writeFloat(this.ladderBuffer, ladder.exit[i], this.ladderBufferIndex);
            }

            for (let i = 0; i < 3; i++) {
                this.ladderBufferIndex = this.writer.writeFloat(this.ladderBuffer, ladder.normal[i], this.ladderBufferIndex);
            }
            
            this.ladderBufferIndex = this.writer.writeFloat(this.ladderBuffer, ladder.radius, this.ladderBufferIndex);
        }

        this.ladderArrayIndex += ladders.length;
    }

    _writeBoundingVolumesToBuffer(boundingVolumes) {
        for (const boundingVolume of boundingVolumes) {
            const floorIndex = this.triangleArrayIndex;
            this._writeTrianglesToBuffer(boundingVolume.floors);

            const wallIndex = this.triangleArrayIndex;
            this._writeTrianglesToBuffer(boundingVolume.walls);

            const ceilingIndex = this.triangleArrayIndex;
            this._writeTrianglesToBuffer(boundingVolume.ceilings);

            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, boundingVolume.type, this.boundingVolumesBufferIndex);
            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, boundingVolume.node.index, this.boundingVolumesBufferIndex);
            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, floorIndex, this.boundingVolumesBufferIndex);
            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, boundingVolume.floors.length, this.boundingVolumesBufferIndex);
            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, wallIndex, this.boundingVolumesBufferIndex);
            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, boundingVolume.walls.length, this.boundingVolumesBufferIndex);
            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, ceilingIndex, this.boundingVolumesBufferIndex);
            this.boundingVolumesBufferIndex = this.writer.writeUInt16(this.boundingVolumeBuffer, boundingVolume.ceilings.length, this.boundingVolumesBufferIndex);
            
            // Note: when more bounding primitives are supported this will need to be modified
            this.boundingVolumesBufferIndex = boundingVolume.volume.write(this.writer, this.boundingVolumeBuffer, this.boundingVolumesBufferIndex);
        }

        this.boundingVolumesArrayIndex += boundingVolumes.length;
    }
}


module.exports = CollisionGeometryWriter;