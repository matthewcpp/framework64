const DisplayList = require("./DisplayList");
const N64Material = require("./N64Material");
const N64Primitive = require("./N64Primitive");
const N64Defs = require("./N64Defs");
const N64Slicer = require("./N64Slicer");
const VertexBuffer = require("./VertexBuffer");
const Bounding = require("./Bounding");
const Util = require("../Util");
const GLTFVertexIndex = require("../GLTFVertexIndex")
const MaterialBundleWriter = require("./MaterialBundleWriter");

const processImage = require("./ProcessImage");

const path  = require("path");
const fs = require("fs");


/** Writes a self contained static mesh to file. */
async function writeStaticMesh(staticMesh, destPath) {
    const file = fs.openSync(destPath, "w");
    await writeStaticMeshToFile(staticMesh, file);
    fs.closeSync(file);
}

async function writeStaticMeshToFile(staticMesh, file) {
    if (staticMesh.materialBundle === null) {
        throw new Error("Error writing static mesh: no material bundle present on mesh.");
    }

    await _writeMeshToFile(staticMesh, null, null, file);
}

/** Wrties a static mesh to a currently open file stream */
async function writeStaticMeshData(mesh, materialBundle, n64Images, file) {
    await _writeMeshToFile(mesh, materialBundle, n64Images, file)
}

async function _writeMeshToFile(mesh, materialBundle, bundleImages, file) {
    let n64Images = bundleImages;

    if (mesh.materialBundle) {
        materialBundle = mesh.materialBundle;
        n64Images = await createN64Images(materialBundle.gltfData);
    }

    adjustN64VertexNormals(mesh);
    adjustN64TexCoordinates(mesh, materialBundle.gltfData, n64Images);
    adjustN64VertexColors(mesh);

    const meshInfo = new MeshInfo();
    meshInfo.materialBundle = mesh.materialBundle;
    meshInfo.primitiveCount = mesh.primitives.length;
    meshInfo.vertexPointerDataSize = mesh.primitives.length * 4;
    meshInfo.bounding = mesh.bounding;

    const primitiveInfos = [];
    const vertexBuffers = [];
    const displayListBuffers = [];
    const vertexPointerBuffers = []
    const vertexPointerCountBuffer = Buffer.alloc(mesh.primitives.length * 4); // holds number of vertex pointer indices per primitive

    for (let i = 0; i <  mesh.primitives.length; i++) {
        const primitive = mesh.primitives[i];

        const primitiveInfo = new PrimitiveInfo();
        primitiveInfo.verticesBufferIndex = meshInfo.vertexCount; // the index for this primitive is set to the total size of the mesh's vertices buffer added thus far
        primitiveInfo.displayListBufferIndex = meshInfo.displayListCount; // the index for this primitive is set to the total size of the mesh's display list buffer this far
        primitiveInfo.material = materialBundle.getBundledMaterialIndex(primitive.material);
        primitiveInfo.jointIndex = primitive.jointIndices ? primitive.jointIndices[0] : N64Primitive.NoJoint; // used for skinning, refer to N64Mesh.splitPrimitivesForSkinning
        primitiveInfos.push(primitiveInfo);

        // slice the vertices into chunks that can fit into N64 vertex cache
        const slices = N64Slicer.slice(primitive);

        // generate the display list for rendering the primitive geometry
        const vertexBuffer = VertexBuffer.createVertexBuffer(slices, primitive.hasNormals);
        const {displayList, vertexPointers, vertexCommandIndices} = primitive.elementType === N64Primitive.ElementType.Triangles ?
            DisplayList.createTriangleDisplayListBuffer(slices) : DisplayList.createLineDisplayListBuffer(slices);

        // update the mesh info totals
        meshInfo.vertexCount += vertexBuffer.length / N64Defs.SizeOfVtx;
        meshInfo.displayListCount += displayList.length / N64Defs.SizeOfGfx;
        meshInfo.vertexPointerDataSize += vertexPointers.length;
        vertexPointerCountBuffer.writeUInt32BE(vertexPointers.length / 4, i * 4); // vertexPointers is a binary buffer of uint 32's so we divide to get the actual count of items in the buffer

        vertexBuffers.push(vertexBuffer);
        displayListBuffers.push(displayList);
        vertexPointerBuffers.push(vertexPointers);
    }

    fs.writeSync(file, meshInfo.buffer);
    
    if (mesh.materialBundle) {
        MaterialBundleWriter.write(materialBundle.gltfData, materialBundle, n64Images, file);
    }

    for (const buffer of vertexBuffers)
        fs.writeSync(file, buffer);

    for (const buffer of displayListBuffers)
        fs.writeSync(file, buffer);

    for (const primitiveInfo of primitiveInfos) {
        fs.writeSync(file, primitiveInfo.buffer)
    }

    fs.writeSync(file, vertexPointerCountBuffer);
    for (const buffer of vertexPointerBuffers)
        fs.writeSync(file, buffer);
}

/**
 * This class represents the header of the Mesh data.
 * It is read first and describes the counts of the various components of the mesh.
 * The generated buffer should be kept in sync with the fw64MeshInfo struct in include/n64/mesh.h
 * */
class MeshInfo {
    primitiveCount = 0;
    vertexCount = 0;
    displayListCount = 0;
    vertexPointerDataSize = 0;
    bounding = null;
    materialBundle = null;

    get buffer() {
        const buff = Buffer.alloc(20 + Bounding.SizeOf);
        let index = 0;

        index = buff.writeUInt32BE(this.primitiveCount, index);
        index = buff.writeUInt32BE(this.vertexCount, index);
        index = buff.writeUInt32BE(this.displayListCount, index);
        index = buff.writeUInt32BE(this.vertexPointerDataSize, index);
        index = buff.writeUInt32BE(this.materialBundle != null ? 1 : 0, index);
        index = this.bounding.writeToBuffer(buff, index);

        return buff;
    }
}

/**
 * This class represents a single primitive in the mesh.
 * It contains indices into the various mesh-level arrays
 */
class PrimitiveInfo {
    verticesBufferIndex;
    displayListBufferIndex;
    material; // index into mesh's material array
    jointIndex;

    get buffer() {
        const buff = Buffer.alloc(16)

        let index = 0;
        index = buff.writeUInt32BE(this.verticesBufferIndex, index);
        index = buff.writeUInt32BE(this.displayListBufferIndex, index);
        index = buff.writeUInt32BE(this.material, index);
        index = buff.writeUInt32BE(this.jointIndex, index);

        return buff;
    }
}

/** adjusts vertex normals from GLTF (float) to work with N64 vertex format.
 *  the normals are treated as 8-bit signed values (-128 to 127)
*/
function adjustN64VertexNormals(mesh) {
    for (const primitive of mesh.primitives) {
        if (!primitive.hasNormals)
            continue;

        for (const vertex of primitive.vertices) {
            vertex[6] = Math.min(Math.round(vertex[6] * 128), 127);
            vertex[7] = Math.min(Math.round(vertex[7] * 128), 127);
            vertex[8] = Math.min(Math.round(vertex[8] * 128), 127);
            vertex[9] = 255
        }
    }
}

/** adjusts tex coords from GLTF (float) to work with N64 tex coord system */
function adjustN64TexCoordinates(mesh, gltfData, n64Images) {
    /** ensure that the tex coord value will sit in a signed 16 bit integer */
    const validateTexCoord = (val) => {
        return val >= -32768 && val <= 32767;
    }

    for (const primitive of mesh.primitives) {
        if (!primitive.hasTexCoords)
            continue;

        const material = gltfData.materials[primitive.material];
        if (material.texture === N64Material.NoTexture)
            continue;
        
        const texture = gltfData.textures[material.texture];
        const image = n64Images[texture.image];

        if (!Util.isPowerOf2(image.width) || !Util.isPowerOf2(image.height)) {
            throw new Error(`image: ${image.name} has non power of 2 dimensions: ${image.width}x${image.height}`);
        }

        for (const vertex of primitive.vertices) {
            let s = vertex[4];
            let t = vertex[5];

            s *= image.width * 2;
            t *= image.height * 2;

            // Note that the texture coordinates (s,t) are encoded in S10.5 format.
            vertex[4] = Math.round(s * (1 << 5));
            vertex[5] = Math.round(t * (1 << 5));

            const textCoordsAreValid = validateTexCoord(vertex[4]) && validateTexCoord(vertex[5]);

            if (!textCoordsAreValid) {
                throw new Error(`Mesh ${mesh.name}: Detected a tex coord value outside of S10.5 format range.  Check model source data.`);
            }
        }
    }
}

/** adjusts tex coords from GLTF (float) 0.0-1.0 to uint8 0-255 */
function adjustN64VertexColors(mesh) {
    for (const primitive of mesh.primitives) {
        if (!primitive.hasVertexColors) {
            continue;
        }

        for (const vertex of primitive.vertices) {
            vertex[GLTFVertexIndex.ColorR] = Math.round(vertex[GLTFVertexIndex.ColorR] * 255);
            vertex[GLTFVertexIndex.ColorG] = Math.round(vertex[GLTFVertexIndex.ColorG] * 255);
            vertex[GLTFVertexIndex.ColorB] = Math.round(vertex[GLTFVertexIndex.ColorB] * 255);
            vertex[GLTFVertexIndex.ColorA] = Math.round(vertex[GLTFVertexIndex.ColorA] * 255);
        }
    }
}

async function createN64Images(gltfData) {
    const n64Images = [];
    const gltfDir = path.dirname(gltfData.gltfPath);

    for (const imageJson of gltfData.images) {
        n64Images.push(await processImage(imageJson, null, gltfDir, null));
    }

    return n64Images;
}

module.exports = {
    writeStaticMesh: writeStaticMesh,
    writeStaticMeshToFile: writeStaticMeshToFile,
    writeStaticMeshData: writeStaticMeshData,
    createN64Images: createN64Images
};
