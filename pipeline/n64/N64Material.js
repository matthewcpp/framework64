class N64Material {
    static NoTexture = 0xFFFF;

    static ShadingMode  = {
        Unset: 0,
        UnlitVertexColors: 1,
        Gouraud: 3,
        GouraudTextured: 4
    }

    constructor() {
        this.color = [125, 125, 125, 255];
        this.texture = N64Material.NoTexture; // index into the mesh's texture array
        this.textureFrame = 0;
        this.shadingMode = N64Material.ShadingMode.Unset;
    }

    setShadingMode(primitive) {
        if (primitive.hasNormals) {
            this.shadingMode = this.texture !== N64Material.NoTexture ?
                N64Material.ShadingMode.GouraudTextured: N64Material.ShadingMode.Gouraud;
        }
        else if (primitive.hasVertexColors) {
            this.shadingMode = N64Material.ShadingMode.UnlitVertexColors;
        }
        else {
            throw new Error("Could not determine shading mode for primitive");
        }
    }

    get buffer() {
        const buff = Buffer.alloc(16)

        let index = 0;

        index = buff.writeUInt8(this.color[0], index);
        index = buff.writeUInt8(this.color[1], index);
        index = buff.writeUInt8(this.color[2], index);
        index = buff.writeUInt8(this.color[3], index);

        index = buff.writeUInt32BE(this.texture, index);
        index = buff.writeUInt32BE(this.textureFrame, index);
        index = buff.writeUInt32BE(this.shadingMode, index);

        return buff;
    }
}

module.exports = N64Material;