class N64Material {
    static NoTexture = 0xFFFFFFFF;

    constructor() {
        this.ambient = [100, 100, 100]
        this.color = [125, 125, 125];
        this.lightDirection = [40, 40, 40];
        this.texture = N64Material.NoTexture;
    }

    get colorBuffer() {
        const buff = Buffer.alloc(24)

        let index = 0;

        // ambient x2
        for (let i = 0; i < 2; i++) {
            index = buff.writeUInt8(this.ambient[0], index);
            index = buff.writeUInt8(this.ambient[1], index);
            index = buff.writeUInt8(this.ambient[2], index);
            index = buff.writeUInt8(0, index);
        }

        // diffuse light value x2
        for (let i = 0; i < 2; i++) {
            index = buff.writeUInt8(this.color[0], index);
            index = buff.writeUInt8(this.color[1], index);
            index = buff.writeUInt8(this.color[2], index);
            index = buff.writeUInt8(0, index);
        }

        //light direction
        index = buff.writeInt8(this.lightDirection[0], index);
        index = buff.writeInt8(this.lightDirection[1], index);
        index = buff.writeInt8(this.lightDirection[2], index);

        return buff;
    }
}

module.exports = N64Material;