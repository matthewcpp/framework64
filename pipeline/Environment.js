const BuildInfo = require("./BuildInfo");
const WriteInterface = require("./WriteInterface");
const Writer = require("./WriteInterface");

class Environment {
    writer;

    constructor() {
        this.writer = BuildInfo.isLittleEndian ? WriteInterface.littleEndian() : WriteInterface.bigEndian();
    }

    get buildInfo() {
        return BuildInfo.buildInfo;
    }

    get binaryWriter() {
        return this.writer;
    }
}

module.exports = Environment;