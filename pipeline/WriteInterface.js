class WriteInterface {
    writeFloat;
    writeUInt32;
    writeUint64;
    floatBufferToNative;
    int16ArrayToBuffer;
    floatArrayToBuffer;

    static bigEndian() {
        const i = new WriteInterface();
        i.writeFloat = (buffer, val, offset) => { return buffer.writeFloatBE(val, offset); };
        i.writeUInt16 = (buffer, val, offset) => { return buffer.writeUInt16BE(val, offset); };
        i.writeUInt32 = (buffer, val, offset) => { return buffer.writeUInt32BE(val, offset); };
        i.writeUInt64 = (buffer, val, offset) => { return buffer.writeBigUInt64BE(BigInt(val), offset); };
        
        i.floatBufferToNative = (buffer) => {
            const bufferBE = Buffer.alloc(buffer.length);
            for (let i = 0; i < buffer.length; i += 4) {
                bufferBE.writeFloatBE(buffer.readFloatLE(i), i);
            }
            return bufferBE;
        }
        i.int16ArrayToBuffer = (array) => {
            const bufferBE = Buffer.alloc(array.length * 2);
            let offset = 0;
            for (const val of array)
                offset = bufferBE.writeUInt16BE(val, offset);
            return bufferBE;
        }
        i.floatArrayToBuffer = (array) => {
            const bufferBE = Buffer.alloc(array.length * 4);
            let offset = 0;
            for (const val of array)
                offset = bufferBE.writeFloatBE(val, offset);
            return bufferBE;
        }

        return i;
    }

    static littleEndian() {
        const i = new WriteInterface();
        i.writeFloat = (buffer, val, offset) => { return buffer.writeFloatLE(val, offset); };
        i.writeUInt16 = (buffer, val, offset) => { return buffer.writeUInt16LE(val, offset); };
        i.writeUInt32 = (buffer, val, offset) => { return buffer.writeUInt32LE(val, offset); };
        i.writeUInt64 = (buffer, val, offset) => { return buffer.writeBigUInt64LE(BigInt(val), offset); };
        
        i.floatBufferToNative = (buffer) => { return buffer; }
        i.int16ArrayToBuffer = (array) => {
            const bufferLE = Buffer.alloc(array.length * 2);
            let offset = 0;
            for (const val of array)
                offset = bufferLE.writeUInt16LE(val, offset);
            return bufferLE;
        }
        i.floatArrayToBuffer = (array) => {
            const bufferLE = Buffer.alloc(array.length * 4);
            let offset = 0;
            for (const val of array)
                offset = bufferLE.writeFloatLE(val, offset);
            return bufferLE;
        }

        return i;
    }
}

module.exports = WriteInterface;