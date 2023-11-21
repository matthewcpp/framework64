class Material {
    static NoTexture = 0xFFFFFFFF;

    /** this needs to match up with fw64ShadingMode defined in material.h*/
    static ShadingMode  = {
        Unset: 0,
        VertexColors: 1,
        VertexColorsTextured: 2,
        Gouraud: 3,
        GouraudTextured: 4,
        UnlitTextured: 5,
        DecalTexture: 6
    }

    color = [255, 255, 255, 255];

    /** index into the mesh's texture array */
    texture = Material.NoTexture;
    textureFrame = 0;
    shadingMode = Material.ShadingMode.Unset;

    hasTexture() {
        return this.texture !== Material.NoTexture;
    }

    setColorFromFloatArray(baseColor) {
        this.color[0] = Math.round(baseColor[0] * 255);
        this.color[1] = Math.round(baseColor[1] * 255);
        this.color[2] = Math.round(baseColor[2] * 255);
        this.color[3] = Math.round(baseColor[3] * 255);
    }
}

module.exports = Material;