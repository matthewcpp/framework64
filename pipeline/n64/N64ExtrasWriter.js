const N64SceneExtra = require("./N64SceneExtra");

const fs = require("fs");

function writeExtras(extras, typemap, file) {
    const buffer = Buffer.alloc(extras.length * N64SceneExtra.SizeOf);

    let index = 0;

    for (const extra of extras) {
        if (!typemap.hasOwnProperty(extra.type)) {
            throw new Error(`Error processing scene extra : Unknown type: ${extra.type}`);
        }

        const typeId = typemap[extra.type];

        index = buffer.writeUInt32BE(typeId, index);
        index = buffer.writeFloatBE(extra.position[0], index);
        index = buffer.writeFloatBE(extra.position[1], index);
        index = buffer.writeFloatBE(extra.position[2], index);

        index = buffer.writeFloatBE(extra.rotation[0], index);
        index = buffer.writeFloatBE(extra.rotation[1], index);
        index = buffer.writeFloatBE(extra.rotation[2], index);
        index = buffer.writeFloatBE(extra.rotation[3], index);

        index = buffer.writeFloatBE(extra.scale[0], index);
        index = buffer.writeFloatBE(extra.scale[1], index);
        index = buffer.writeFloatBE(extra.scale[2], index);
    }

    return fs.writeSync(file, buffer);
}

module.exports = {
    write: writeExtras
}