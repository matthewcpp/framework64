const fs = require("fs");
const path = require("path");

if (process.argv.length < 4) {
	console.log("Usage: node gltf2n64 /path/to/model.gltf /path/to/output/folder");
	process.exit(1);
}

const gltfPath = process.argv[2];
const outputFolder = process.argv[3];

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
		this.texture = null;
	}
}

class N64Model {
	constructor(name) {
		this.name = name;
		this.meshes = [];
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

			triangleOffset += mesh.vertices.length;
		}
	}
}

const modelName = path.basename(gltfPath, ".gltf");
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

function readPositions(gltfPrimitive, n64Mesh) {
	const accessor = gltf.accessors[gltfPrimitive.attributes.POSITION];
	const bufferView = gltf.bufferViews[accessor.bufferView];
	const buffer = getBuffer(bufferView.buffer);

	const stride = bufferView.hasOwnProperty("byteStride") ? bufferView.byteStride : 4;
	let offset = bufferView.byteOffset;

	for (let i = 0; i < accessor.count; i++) {
		const p0 = parseInt(buffer.readFloatLE(offset) * globalScale);
		offset += stride;

		const p1 = parseInt(buffer.readFloatLE(offset) * globalScale);
		offset += stride;

		const p2 = parseInt(buffer.readFloatLE(offset) * globalScale);
		offset += stride;

		n64Mesh.vertices.push([p0, p1, p2, 0]);
		console.log("push vertex");
	}
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

function readIndices(gltfPrimitive, n64Mesh) {
	const accessor = gltf.accessors[gltfPrimitive.indices];
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
		console.log("push triangle")
	}
}


for (const gltfMesh of gltf.meshes) {

	for (const gltfPrimitive of gltfMesh.primitives) {
		// only support triangles for now
		if (gltfPrimitive.hasOwnProperty("mode") && gltfPrimitive.mode != 4)
			continue;

			console.log ("process mesh");
		const n64Mesh = new N64Mesh();
		model.meshes.push(n64Mesh);
	
		readPositions(gltfPrimitive, n64Mesh);
		readTexCoords(gltfPrimitive, n64Mesh);
		readMaterial(gltfPrimitive, n64Mesh);
	
		readIndices(gltfPrimitive, n64Mesh);
	}
}

model.mergeVertexColorMeshes();

const filePath = path.join(outputFolder, `${model.name}.h`)
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