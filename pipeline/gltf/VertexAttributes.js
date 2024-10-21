/// This needs to be kept in sync with framework64/mesh.h
class VertexAttributes{
    static None = 0;
    static Positions = 1;
    static Normals = 2;
    static TexCoords = 4;
    static VertexColors = 8;
}

module.exports = VertexAttributes;