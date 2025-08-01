const glMatrix = require("gl-matrix");

class Bounding {
    static SizeOf = 24;

    constructor() {
        this.min = [Number.POSITIVE_INFINITY, Number.POSITIVE_INFINITY, Number.POSITIVE_INFINITY];
        this.max = [Number.NEGATIVE_INFINITY, Number.NEGATIVE_INFINITY, Number.NEGATIVE_INFINITY];
    }

    get extents() {
        const extents = glMatrix.vec3.create();
        glMatrix.vec3.sub(extents, this.max, this.min);
        glMatrix.vec3.scale(extents, extents, 0.5);

        return extents;
    }

    get center() {
        const center = glMatrix.vec3.create();
        glMatrix.vec3.add(center, this.min, this.max);
        glMatrix.vec3.scale(center, center, 0.5);

        return center;
    }

    setFromCenterAndExtents(center, extents) {
        glMatrix.vec3.subtract(this.min, center, extents);
        glMatrix.vec3.add(this.max, center, extents);
    }

    encapsulatePoint(point) {
        for (let i = 0; i < 3; i++) {
            if (point[i] < this.min[i]){
                this.min[i] = point[i];
            }

            if (point[i] > this.max[i]) {
                this.max[i] = point[i];
            }
        }
    }

    encapsulateBox(box) {
        this.encapsulatePoint(box.min);
        this.encapsulatePoint(box.max);
    }

    toArrayStr() {
        return `${this.min[0]}, ${this.min[1]}, ${this.min[2]}, ${this.max[0]}, ${this.max[1]}, ${this.max[2]}`
    }

    writeToBuffer(buffer, index) {
        index = buffer.writeFloatBE(this.min[0], index);
        index = buffer.writeFloatBE(this.min[1], index);
        index = buffer.writeFloatBE(this.min[2], index);
        index = buffer.writeFloatBE(this.max[0], index);
        index = buffer.writeFloatBE(this.max[1], index);
        index = buffer.writeFloatBE(this.max[2], index);

        return index;
    }

    writeToBufferLE(buffer, index) {
        index = buffer.writeFloatLE(this.min[0], index);
        index = buffer.writeFloatLE(this.min[1], index);
        index = buffer.writeFloatLE(this.min[2], index);
        index = buffer.writeFloatLE(this.max[0], index);
        index = buffer.writeFloatLE(this.max[1], index);
        index = buffer.writeFloatLE(this.max[2], index);

        return index;
    }

    write(writer, buffer, index){
        index = writer.writeFloat(buffer, this.min[0], index);
        index = writer.writeFloat(buffer, this.min[1], index);
        index = writer.writeFloat(buffer, this.min[2], index);

        index = writer.writeFloat(buffer, this.max[0], index);
        index = writer.writeFloat(buffer, this.max[1], index);
        index = writer.writeFloat(buffer, this.max[2], index);

        return index;
    }

    static createFromCenterAndExtents(center, extents) {
        const boundingBox = new Bounding();
        boundingBox.setFromCenterAndExtents(center, extents);

        return boundingBox;
    }

    static createFromMinMax(min, max) {
        const boundingBox = new Bounding();
        glMatrix.vec3.copy(boundingBox.min, min);
        glMatrix.vec3.copy(boundingBox.max, max);

        return boundingBox;
    }

    static createFromTriangle(a, b, c) {
        const boundingBox = new Bounding();
        glMatrix.vec3.copy(boundingBox.min, a);
        glMatrix.vec3.copy(boundingBox.max, a);

        glMatrix.vec3.min(boundingBox.min, boundingBox.min, b);
        glMatrix.vec3.max(boundingBox.max, boundingBox.max, b);
        glMatrix.vec3.min(boundingBox.min, boundingBox.min, c);
        glMatrix.vec3.min(boundingBox.max, boundingBox.max, c);

        return boundingBox;
    }

    // https://gist.github.com/cmf028/81e8d3907035640ee0e3fdd69ada543f
    static transformByMatrix(box, matrix) {
        const corners = [];
        corners[0] = box.min;
        corners[1] = [box.min[0], box.max[1], box.min[2]];
        corners[2] = [box.min[0], box.max[1], box.max[2]];
        corners[3] = [box.min[0], box.min[1], box.max[2]];
        corners[4] = [box.max[0], box.min[1], box.min[2]];
        corners[5] = [box.max[0], box.max[1], box.min[2]];
        corners[6] = box.max;
        corners[7] = [box.max[0], box.min[1], box.max[2]];
            
        // transform the first corner
        const tmin = glMatrix.vec3.create();
        glMatrix.vec3.transformMat4(tmin, corners[0], matrix);
        const tmax = glMatrix.vec3.clone(tmin);
        
        // transform the other 7 corners and compute the result AABB
        const xformedPoint = glMatrix.vec3.create();
        for(let i = 1; i < 8; i++)
        {
            glMatrix.vec3.transformMat4(xformedPoint, corners[i], matrix);

            glMatrix.vec3.min(tmin, tmin, xformedPoint);
            glMatrix.vec3.max(tmax, tmax, xformedPoint);
        }

        return Bounding.createFromMinMax(tmin, tmax);
    }
}

module.exports = Bounding;
