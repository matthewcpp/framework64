const Bounding = require("./Bounding");

class N64Model {
    constructor(name) {
        this.name = name;
        this.meshes = [];
    }

    get bounding() {
        const bounding = new Bounding();

        for (const mesh of this.meshes) {
            bounding.encapsulateBox(mesh.bounding);
        }

        return bounding;
    }

    mergeVertexColorMeshes() {
        const meshList = this.meshes;
        const mergeMesh = new N64Mesh();
        this.meshes = [mergeMesh];

        let triangleOffset = 0;

        for (const mesh of meshList) {
            if (mesh.texture != null) {
                this.meshes.push(mesh);
                continue;
            }

            mergeMesh.vertices.push(...mesh.vertices);
            
            for (const t of mesh.triangles) {
                mergeMesh.triangles.push([ t[0] + triangleOffset, t[1] + triangleOffset, t[2] + triangleOffset]);
            }

            mergeMesh.bounding.encapsulateBox(mesh.bounding);

            triangleOffset += mesh.vertices.length;
        }
    }
}

module.exports = N64Model;
