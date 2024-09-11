class Material {
    static NoTexture = 0xFFFFFFFF;

    /** this needs to match up with fw64ShadingMode defined in material.h*/
    static ShadingMode  = {
        Unset: 0,
        Unlit: 1,
        UnlitTextured: 2,
        Lit: 3,
        LitTextured: 4,
        DecalTexture: 5,
        Line: 6,
        UnlitTransparentTextured: 7
    }

    /* note: this is specified as RGBA32 */
    color = [255, 255, 255, 255];

    /** index into the mesh's texture array */
    texture = Material.NoTexture;
    textureFrame = 0;
    shadingMode = Material.ShadingMode.Unset;

    hasTexture() {
        return this.texture !== Material.NoTexture;
    }

    get usesNormals() {
        return this.shadingMode === Material.ShadingMode.Lit || this.shadingMode === Material.ShadingMode.LitTextured;
    }

    get usesVertexColors() {
        return !this.usesNormals;
    }

    get hasAlpha() {
        return this.color[3] < 255;
    }

    setColorFromFloatArray(baseColor) {
        this.color[0] = Math.round(baseColor[0] * 255);
        this.color[1] = Math.round(baseColor[1] * 255);
        this.color[2] = Math.round(baseColor[2] * 255);
        this.color[3] = Math.round(baseColor[3] * 255);
    }
}

module.exports = Material;