const { program } = require('commander');

const fs = require("fs");
const path = require("path");

program.version("0.0.1");
program.requiredOption("-f, --file <path>", "input file");
program.requiredOption("-o, --out-dir <dir>", "output directory");
program.option("-m --merge", "attempt to merge meshes when possible", false);
program.option("-s --scale <value>", "scale value to apply to all vertices", "1.0");

program.parse(process.argv);

const gltfPath = program.file;
const outputFolder = program.outDir;
const globalScale = parseFloat(program.scale);

if (!fs.existsSync(gltfPath)) {
    console.log(`Unable to open: ${gltfPath}`)
    process.exit(1);
}

console.log(`Processing: ${gltfPath}`);

const gltf = JSON.parse(fs.readFileSync(gltfPath, {encoding: "utf8"}));

const loadedBuffers = new Map();

const vertexSliceSize = 30;

class Bounding {
    constructor() {
        this.min = [Number.MAX_VALUE, Number.MAX_VALUE, Number.MAX_VALUE];
        this.max = [Number.MIN_VALUE, Number.MIN_VALUE, Number.MIN_VALUE];
    }

    encapsulatePoint(point) {
        for (let i = 0; i < 3; i++) {
            if (point[i] < this.min[i]){
                this.min[i] = point[i];
            }
                

            if (point[i] > this.max[i]) {
                this.max[i] = point[i];
            }
        }
    }

    encapsulateBox(box) {
        this.encapsulatePoint(box.min);
        this.encapsulatePoint(box.max);
    }

    toArrayStr() {
        return `${this.min[0]}, ${this.min[1]}, ${this.min[2]}, ${this.max[0]}, ${this.max[1]}, ${this.max[2]}`
    }
}

class N64Mesh {
    constructor() {
        this.vertices = [];
        this.triangles = [];
        this.texture = null;

        this.bounding = new Bounding();
    }

    slice() {
        // if the mesh is small, then there is only one slice
        if (this.vertices.length <= vertexSliceSize) {
            return [{vertices: this.vertices, triangles: this.triangles}]
        }

        const slices = [];

        // parition all vertices into their slices
        for (let i = 0; i < this.vertices.length; i++) {
            if (i % vertexSliceSize === 0) {
                slices.push({vertices: [], triangles: []});
            }

            slices[slices.length - 1].vertices.push(this.vertices[i]);
        }

        const getTriangleSliceIndex = (triangle) => {
            let index = -1;
            for (let i = 0; i < slices.length; i++) {
                const minIndex = i * vertexSliceSize;
                const maxIndex = (i + 1) * vertexSliceSize;

                // triangle fits in this slice?
                if ((triangle[0] >= minIndex && triangle[0] < maxIndex) &&
                    (triangle[1] >= minIndex && triangle[1] < maxIndex) &&
                    (triangle[2] >= minIndex && triangle[2] < maxIndex)) {
                        index = i;
                        break;
                    }
            }

            return index;
        }

        // sort all triangles into their slices
        for (const triangle of this.triangles) {
            const sliceIndex = getTriangleSliceIndex(triangle);
            if (sliceIndex === -1) {
                continue;
            }
            // adjust triangle indices and add to triangle list
            slices[sliceIndex].triangles.push([triangle[0] - sliceIndex * vertexSliceSize, triangle[1] - sliceIndex * vertexSliceSize, triangle[2] - sliceIndex * vertexSliceSize]);
        }

        return slices;
    }
}

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

const modelName = path.basename(gltfPath, ".gltf");
const model = new N64Model(modelName);

function getBuffer(bufferIndex) {
    let  buffer = loadedBuffers.get(bufferIndex);

    if (!buffer) {
        const gltfBuffer = gltf.buffers[bufferIndex];

        const bufferDir = path.dirname(gltfPath);
        const bufferPath = path.join(bufferDir, gltfBuffer.uri);
        console.log(`Load Buffer: ${bufferPath}`);
        buffer = fs.readFileSync(bufferPath);
        loadedBuffers.set(bufferIndex, buffer);
    }

    return buffer;
}

function readPositions(gltfPrimitive, n64Mesh) {
    const accessor = gltf.accessors[gltfPrimitive.attributes.POSITION];
    const bufferView = gltf.bufferViews[accessor.bufferView];
    const buffer = getBuffer(bufferView.buffer);

    const byteStride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : 12;

    let offset = bufferView.byteOffset;

    for (let i = 0; i < accessor.count; i++) {
        const p0 = parseInt(buffer.readFloatLE(offset) * globalScale);
        const p1 = parseInt(buffer.readFloatLE(offset + 4) * globalScale);
        const p2 = parseInt(buffer.readFloatLE(offset + 8) * globalScale);

        n64Mesh.vertices.push([p0, p1, p2, 0]);
        offset += byteStride;
    }

    n64Mesh.bounding.min = accessor.min.slice();
    n64Mesh.bounding.max = accessor.max.slice();
}

/*
inline fixed_point_t float_to_fixed(double input)
{
    return (fixed_point_t)(round(input * (1 << FIXED_POINT_FRACTIONAL_BITS)));
}
*/

function readTexCoords(gltfPrimitive, n64Mesh) {
    for (const vertex of n64Mesh.vertices) {
        vertex.push(0, 0);
    }
}

function readMaterial(gltfPrimitive, n64Mesh) {
    const material = gltf.materials[gltfPrimitive.material];
    const baseColor = material.pbrMetallicRoughness.baseColorFactor;
    const rgba = [parseInt(baseColor[0] * 255), parseInt(baseColor[1] * 255), parseInt(baseColor[2] * 255), parseInt(baseColor[3] * 255)];

    for (const vertex of n64Mesh.vertices) {
        vertex.push(...rgba);
    }
}

function readUShortTriangleList(buffer, byteOffset, count, mesh) {
    let offset = byteOffset;

    for (let i = 0; i < count; i+=3) {
        const p0 = buffer.readUInt16LE(offset);
        offset += 2;

        const p1 = buffer.readUInt16LE(offset);
        offset += 2;

        const p2 = buffer.readUInt16LE(offset);
        offset += 2;

        mesh.triangles.push([p0, p1, p2]);
    }
}

function readUIntTriangleList(buffer, byteOffset, count, mesh) {
    let offset = byteOffset;

    for (let i = 0; i < count; i+=3) {
        const p0 = buffer.readUInt32LE(offset);
        offset += 4;

        const p1 = buffer.readUInt32LE(offset);
        offset += 4;

        const p2 = buffer.readUInt32LE(offset);
        offset += 4;

        mesh.triangles.push([p0, p1, p2]);
    }
}

function readIndices(gltfPrimitive, n64Mesh) {
    const accessor = gltf.accessors[gltfPrimitive.indices];
    const bufferView = gltf.bufferViews[accessor.bufferView];
    const buffer = getBuffer(bufferView.buffer);

    switch (accessor.componentType) {
        case 5123:
            readUShortTriangleList(buffer, bufferView.byteOffset, accessor.count, n64Mesh);
            break;
        case 5125:
            readUIntTriangleList(buffer, bufferView.byteOffset, accessor.count, n64Mesh);
            break;

        default:
            throw new Error(`Unknown Component Type: ${accessor.componentType}`);
    }
}


for (const gltfMesh of gltf.meshes) {

    for (const gltfPrimitive of gltfMesh.primitives) {
        // only support triangles for now
        if (gltfPrimitive.hasOwnProperty("mode") && gltfPrimitive.mode != 4)
            continue;

        const n64Mesh = new N64Mesh();
        model.meshes.push(n64Mesh);
    
        readPositions(gltfPrimitive, n64Mesh);
        readTexCoords(gltfPrimitive, n64Mesh);
        readMaterial(gltfPrimitive, n64Mesh);
    
        readIndices(gltfPrimitive, n64Mesh);
    }
}

if (program.merge) {
    model.mergeVertexColorMeshes();
}

const filePath = path.join(outputFolder, `${model.name}.h`)
console.log(`write: ${filePath}`);

const file = fs.openSync(filePath, "w");
fs.writeSync(file, `#ifndef MODEL_${modelName.toUpperCase()}_H\n`);
fs.writeSync(file, `#define MODEL_${modelName.toUpperCase()}_H\n\n`);

fs.writeSync(file, `const float ${modelName}_bounding_box[] = { ${model.bounding.toArrayStr()} };\n\n`);

for (let i = 0; i < model.meshes.length; i++) {
    console.log(`Mesh: ${model.meshes[i].vertices.length} vertices, ${model.meshes[i].triangles.length} triangles`);
    const slices = model.meshes[i].slice();
    const vertexArrayVar = `${modelName}_mesh_${i}`;

    // Write out mesh vertices for all slices in this mesh
    fs.writeSync(file, `const Vtx ${vertexArrayVar}[] = {\n`);
    for (const slice of slices) {
        for (const v of slice.vertices) {
            fs.writeSync(file, `{ ${v[0]}, ${v[1]}, ${v[2]}, ${v[3]}, ${v[4]}, ${v[5]}, ${v[6]}, ${v[7]}, ${v[8]}, ${v[9]} },\n`)
        }
    }
    fs.writeSync(file, `};\n\n`);

    // write out the mesh display list
    const displayListVar = vertexArrayVar + "_dl"
    fs.writeSync(file, `const Gfx ${displayListVar}[] = {\n`);

    for (let i = 0; i < slices.length; i++) {
        const slice = slices[i];
        // set the vertex pointer
        fs.writeSync(file, `gsSPVertex(${vertexArrayVar} + ${vertexSliceSize * i}, ${slice.vertices.length}, 0),\n`);

        // write out all the triangles
        for (const t of slice.triangles) {
            fs.writeSync(file, `gsSP1Triangle(${t[0]}, ${t[1]}, ${t[2]}, 0),\n`)
        }
    }

    // end the display list
    fs.writeSync(file, "gsSPEndDisplayList()\n");
    fs.writeSync(file, `};\n\n`);
}

fs.writeSync(file, `#endif`);
fs.closeSync(file);