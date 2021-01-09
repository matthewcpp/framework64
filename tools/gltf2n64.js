const fs = require("fs");
const path = require("path");

if (process.argv.length < 3) {
	console.log("Usage: node gltf2n64 /path/to/model.gltf");
	process.exit(1);
}

const gltfPath = process.argv[2];

if (!fs.existsSync(gltfPath)) {
	console.log(`Unable to open: ${gltfPath}`)
	process.exit(1);
}

console.log(`Processing: ${gltfPath}`);

const gltf = JSON.parse(fs.readFileSync(gltfPath, {encoding: "utf8"}));

const loadedBuffers = new Map();

class N64Mesh {
	constructor() {
		this.vertices = [];
		this.triangles = [];
	}
}

class N64Model {
	constructor(name) {
		this.name = name;
		this.meshes = [];
	}
}

const modelName = path.basename(gltfPath, ".gltf");
const modelDir = path.dirname(gltfPath);
const model = new N64Model(modelName);
const globalScale = 1;

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

function readPositions(gltfMesh, n64Mesh) {
	const accessor = gltf.accessors[gltfMesh.primitives[0].attributes.POSITION];
	const bufferView = gltf.bufferViews[accessor.bufferView];
	const buffer = getBuffer(bufferView.buffer);

	const stride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : 4;
	let offset = bufferView.byteOffset;

	for (let i = 0; i < accessor.count; i+=3) {
		const p0 = parseInt(buffer.readFloatLE(offset) * globalScale);
		offset += stride;

		const p1 = parseInt(buffer.readFloatLE(offset) * globalScale);
		offset += stride;

		const p2 = parseInt(buffer.readFloatLE(offset) * globalScale);
		offset += stride;

		n64Mesh.vertices.push([p0, p1, p2, 0]);
	}
}

/*
inline fixed_point_t float_to_fixed(double input)
{
    return (fixed_point_t)(round(input * (1 << FIXED_POINT_FRACTIONAL_BITS)));
}
*/

function readTexCoords(gltfMesh, n64Mesh) {
	for (const vertex of n64Mesh.vertices) {
		vertex.push(0, 0);
	}
}

function readColors(gltfMesh, n64Mesh) {
	for (const vertex of n64Mesh.vertices) { 
		for (let i = 0; i < 3; i ++) {
			vertex.push(parseInt(Math.random() * (255 - 30) + 30));
		}

		vertex.push(255);
	}
	
}

function readIndices(gltfMesh, n64Mesh) {
	const accessor = gltf.accessors[gltfMesh.primitives[0].indices];
	const bufferView = gltf.bufferViews[accessor.bufferView];
	const buffer = getBuffer(bufferView.buffer);

	const stride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : 2
	let offset = bufferView.byteOffset;

	for (let i = 0; i < accessor.count; i+=3) {
		const p0 = parseInt(buffer.readUInt16LE(offset));
		offset += stride;

		const p1 = parseInt(buffer.readUInt16LE(offset));
		offset += stride;

		const p2 = parseInt(buffer.readUInt16LE(offset));
		offset += stride;

		n64Mesh.triangles.push([p0, p1, p2]);
	}
}


for (const gltfMesh of gltf.meshes) {
	const n64Mesh = new N64Mesh();
	model.meshes.push(n64Mesh);

	readPositions(gltfMesh, n64Mesh);
	readTexCoords(gltfMesh, n64Mesh);
	readColors(gltfMesh, n64Mesh);

	readIndices(gltfMesh, n64Mesh);
}

const filePath = path.join(modelDir, `${modelName}.h`)
console.log(`write: ${filePath}`);
const file = fs.openSync(filePath, "w");
fs.writeSync(file, `#ifndef MODEL_${modelName.toUpperCase()}_H\n`);
fs.writeSync(file, `#define MODEL_${modelName.toUpperCase()}_H\n\n`);

for (let i = 0; i < model.meshes.length; i++) {
	const mesh = model.meshes[i];
	const vertexArrayVar = `${modelName}_mesh_${i}`;

	fs.writeSync(file, `const Vtx ${vertexArrayVar}[] = {\n`);
	for (let j = 0; j < mesh.vertices.length; j++) {
		const v = mesh.vertices[j];
		fs.writeSync(file, `{ ${v[0]}, ${v[1]}, ${v[2]}, ${v[3]}, ${v[4]}, ${v[5]}, ${v[6]}, ${v[7]}, ${v[8]}, ${v[9]} },\n`)
	}
	fs.writeSync(file, `};\n\n`);

	const displayListVar = vertexArrayVar + "_dl"
	fs.writeSync(file, `const Gfx ${displayListVar}[] = {\n`);
	fs.writeSync(file, `gsSPVertex(${vertexArrayVar},${mesh.vertices.length},0),\n`);

	for (let j = 0; j < mesh.triangles.length; j++) {
		const t = mesh.triangles[j];
		fs.writeSync(file, `gsSP1Triangle(${t[0]}, ${t[1]}, ${t[2]}, 0),\n`)
	}

	fs.writeSync(file, "gsSPEndDisplayList()\n");
	fs.writeSync(file, `};\n\n`);
}

fs.writeSync(file, `#endif`);
fs.closeSync(file);