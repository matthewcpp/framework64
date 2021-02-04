const Bounding = require("./Bounding");

class N64Model {
    constructor(name) {
        this.name = name;
        this.meshes = [];
        this.materials = [];
        this.images = [];
    }

    get bounding() {
        const bounding = new Bounding();

        for (const mesh of this.meshes) {
            bounding.encapsulateBox(mesh.bounding);
        }

        return bounding;
    }

    get hasNormals() {
        for (const mesh of this.meshes) {
            if (mesh.hasNormals)
                return true;
        }
        return false;
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

    createWireframe(wireColor) {
        const tryAddEdge = function(edgeSet, slice, v0, v1) {
            const point1 = slice.vertices[v0].slice(0,3);
            const point2 = slice.vertices[v1].slice(0,3);
            const hashValue1 = [...point1, ...point2].toString();

            if (edgeSet.has(hashValue1))
                return false;

            const hashValue2 = [...point2, ...point1].toString();
            edgeSet.add(hashValue1);
            edgeSet.add(hashValue2);

            slice.lines.push(v0, v1);

            return true;
        };

        const results = [];
        const edgeSet = new Set();

        for (const mesh of this.meshes) {
            // lines use flat shading only.  Need to set the vertex color
            const vertexColor = !!wireColor ? wireColor : this.materials[mesh.material].ambient;

            const result = mesh.slice();

            for (const slice of result) {
                slice.lines = [];

                for (const triangle of slice.triangles) {
                    tryAddEdge(edgeSet, slice, triangle[0], triangle[1]);
                    tryAddEdge(edgeSet, slice, triangle[1], triangle[2]);
                    tryAddEdge(edgeSet, slice, triangle[2], triangle[0]);
                }


                for (const vertex of slice.vertices) {
                    vertex[6] = vertexColor[0];
                    vertex[7] = vertexColor[1];
                    vertex[8] = vertexColor[2];
                }

                delete slice.triangles;
            }

            results.push(result);
        }

        return results;
    }
}

module.exports = N64Model;
