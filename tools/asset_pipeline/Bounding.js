class Bounding {
    static SizeOf = 24;

    constructor() {
        this.min = [Number.MAX_VALUE, Number.MAX_VALUE, Number.MAX_VALUE];
        this.max = [Number.MIN_VALUE, Number.MIN_VALUE, Number.MIN_VALUE];
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
}

module.exports = Bounding;
