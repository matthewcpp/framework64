const fs = require("fs");
const CollisionGeometry = require("./CollisionGeometry");
const Bounding = require("./gltf/Bounding");

class CollisionGeometryWriter {
    /** This needs to be kept in sync with fw64CollisionGeometryInfo in collision_geometry.h */
    static headerSize = ((2 /*element counts */ + 3 /* cell counts */) * 4)  + Bounding.SizeOf; /* bounding min/max */

    /** This needs to be kept in sync with fw64CollisionTriangle in collision_geometry.h */
    static triangleSize = ((3 * 3) /* points */ + (1 * 3) /* normal */ + 2 /* extents */) * 4;

    /** This needs to be kept in sync with fw64CollisionLadder in collision_geometry.h */
    static ladderSize = ((3 * 2) /* points */ + 3 /* normal */ + 1 /* radius */) * 4

    /** This needs to be kept in sync with fw64CollisionGeometryCell in collision_geometry.h */
    static cellSize = (4 * 2) * 4;

    /**offset (in bytes) into the binary triangle buffer used for writing */
    triangleBufferIndex = 0;

    /** fw64CollisionTriangle index offset into the triangle array. */
    triangleArrayIndex = 0;

    /** offset (in bytes) into the binary cell data used for writing */
    cellBufferIndex = 0;

    ladderArrayIndex = 0;

    /** offset (in bytes) into the binary ladder data used for writing */
    ladderBufferIndex = 0;

    writer;

    cellBuffer;
    tringleBuffer;
    ladderBuffer;

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

        // this needs to line up with fw64CollisionGeometryInfo in collision_geometry.c
        const headerBuffer = Buffer.alloc(CollisionGeometryWriter.headerSize);
        let headerIndex = 0;
        headerIndex = this.writer.writeUInt32(headerBuffer, triangleCount, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, ladderCount, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, collisionGeometry.cellCountX, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, 1, headerIndex); // grid is flat...for now
        headerIndex = this.writer.writeUInt32(headerBuffer, collisionGeometry.cellCountZ, headerIndex);
        headerIndex = collisionGeometry.boundingBox.write(this.writer, headerBuffer, headerIndex);

        this.triangleBuffer = Buffer.alloc(CollisionGeometryWriter.triangleSize * triangleCount);
        this.cellBuffer = Buffer.alloc(CollisionGeometryWriter.cellSize * collisionGeometry.cells.length);
        this.ladderBuffer = Buffer.alloc(CollisionGeometryWriter.ladderSize * ladderCount);

        for (const cell of collisionGeometry.cells) {
            this._writeTrianglesToBuffer(cell.walls);
            this._writeTrianglesToBuffer(cell.floors);
            this._writeTrianglesToBuffer(cell.ceilings);
            this._writeLaddersToBuffer(cell.ladders);
        }

        fs.writeSync(file, headerBuffer);
        fs.writeSync(file, this.triangleBuffer);
        fs.writeSync(file, this.ladderBuffer);
        fs.writeSync(file, this.cellBuffer);
    }

    /** for now each triangle is array of length 4 contianing 4 vectors */
    _writeTrianglesToBuffer(triangles) {
        this.cellBufferIndex = this.writer.writeUInt32(this.cellBuffer, this.triangleArrayIndex, this.cellBufferIndex);
        this.cellBufferIndex = this.writer.writeUInt32(this.cellBuffer, triangles.length, this.cellBufferIndex);

        for (const triangle of triangles) {
            // write out points and vector
            for (let i = 0; i < 4; i++) {
                for (let j = 0; j < 3; j++) {
                    this.triangleBufferIndex = this.writer.writeFloat(this.triangleBuffer, triangle[i][j], this.triangleBufferIndex);
                }
            }

            // write out extents
            this.triangleBufferIndex = this.writer.writeFloat(this.triangleBuffer, triangle[4], this.triangleBufferIndex);
            this.triangleBufferIndex = this.writer.writeFloat(this.triangleBuffer, triangle[5], this.triangleBufferIndex);
        }

        this.triangleArrayIndex += triangles.length;
    }

    _writeLaddersToBuffer(ladders){
        this.cellBufferIndex = this.writer.writeUInt32(this.cellBuffer, this.ladderArrayIndex, this.cellBufferIndex);
        this.cellBufferIndex = this.writer.writeUInt32(this.cellBuffer, ladders.length, this.cellBufferIndex);

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
}


module.exports = CollisionGeometryWriter;