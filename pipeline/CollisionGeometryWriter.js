const fs = require("fs");
const CollisionGeometry = require("./CollisionGeometry");
const Bounding = require("./gltf/Bounding");

class CollisionGeometryWriter {
    /** This needs to be kept in sync with fw64CollisionTriangle in collision_geometry.h */
    static triangleSize = (4 * 3) * 4;

    /** This needs to be kept in sync with fw64CollisionGeometryCell in collision_geometry.h */
    static cellSize = (3 * 2) * 4;

    /**offset (in bytes) into the binary triangle buffer used for writing */
    triangleBufferIndex = 0;

    /** fw64CollisionTriangle index offset into the triangle array. */
    triangleArrayIndex = 0;

    /** offset (in bytes) into the cell array */
    cellBufferIndex = 0;

    writer;

    cellBuffer;
    tringleBuffer;

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

        // this needs to line up with CollisionGeometryHeader in collision_geometry.c
        const headerBuffer = Buffer.alloc(12 + Bounding.SizeOf);
        let headerIndex = 0;
        headerIndex = this.writer.writeUInt32(headerBuffer, triangleCount, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, collisionGeometry.cellCountX, headerIndex);
        headerIndex = this.writer.writeUInt32(headerBuffer, collisionGeometry.cellCountZ, headerIndex);
        headerIndex = collisionGeometry.boundingBox.write(this.writer, headerBuffer, headerIndex);

        this.triangleBuffer = Buffer.alloc(CollisionGeometryWriter.triangleSize * triangleCount);
        this.cellBuffer = Buffer.alloc(CollisionGeometryWriter.cellSize * collisionGeometry.cells.length);

        for (const cell of collisionGeometry.cells) {
            this._writeTrianglesToBuffer(cell.walls);
            this._writeTrianglesToBuffer(cell.floors);
            this._writeTrianglesToBuffer(cell.ceilings);
        }

        fs.writeSync(file, headerBuffer);
        fs.writeSync(file, this.triangleBuffer);
        fs.writeSync(file, this.cellBuffer);
    }

    /** for now each triangle is array of length 4 contianing 4 vectors */
    _writeTrianglesToBuffer(triangles) {
        this.cellBufferIndex = this.writer.writeUInt32(this.cellBuffer, this.triangleArrayIndex, this.cellBufferIndex);
        this.cellBufferIndex = this.writer.writeUInt32(this.cellBuffer, triangles.length, this.cellBufferIndex);

        for (const triangle of triangles) {
            for (let i = 0; i < 4; i++) {
                for (let j = 0; j < 3; j++) {
                    this.triangleBufferIndex = this.writer.writeFloat(this.triangleBuffer, triangle[i][j], this.triangleBufferIndex);
                }
            }
        }

        this.triangleArrayIndex += triangles.length;
    }

    writeDebugTextFile(collisionGeometry, filePath) {
        const file = fs.openSync(filePath, "w");

        fs.writeSync(file, `cells: ${collisionGeometry.cells.length} triangles: ${collisionGeometry.triangleCount}\n`);

        for (const cell of collisionGeometry.cells) {
            fs.writeSync(file, "    floors:\n")
            for (const t of cell.floors) {
                fs.writeSync(file, `        a: [${t[0][0]}, ${t[0][1]}, ${t[0][2]}] b: [${t[1][0]}, ${t[1][1]}, ${t[1][2]}] c: [${t[2][0]}, ${t[2][1]}, ${t[2][2]}]\n`);
            }
        }

        fs.closeSync(file);
    }
}


module.exports = CollisionGeometryWriter;