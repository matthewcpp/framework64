const Bounding = require("./gltf/Bounding");

const fs = require("fs");
const Environment = require("./Environment");

/// The order these values are written should correspond with the SceneInfo struct in framework64/scene.h
function writeSceneInfo(scene, file, writer) {
    const sceneInfoBuffer = Buffer.alloc(36);
    let index = 0;

    index = writer.writeUInt16(sceneInfoBuffer, scene.nodes.length, index);
    index = writer.writeUInt16(sceneInfoBuffer, scene.extraNodeCount, index);
    index = writer.writeUInt16(sceneInfoBuffer, scene.meshBundle.length, index);
    index = writer.writeUInt16(sceneInfoBuffer, scene.extraMeshCount, index);
    index = writer.writeUInt32(sceneInfoBuffer, scene.meshInstanceCount, index);
    index = writer.writeUInt32(sceneInfoBuffer, /* skinned mesh count */ 0, index);
    index = writer.writeUInt32(sceneInfoBuffer, scene.skinnedMeshInstanceCount, index);
    index = writer.writeUInt32(sceneInfoBuffer, scene.colliderCount, index);
    index = writer.writeUInt32(sceneInfoBuffer, scene.collisionMeshes.length, index);
    index = writer.writeUInt32(sceneInfoBuffer, scene.customBoundingBoxes.length, index);
    index = writer.writeUInt32(sceneInfoBuffer, !!scene.materialBundle ? 1 : 0, index);

    fs.writeSync(file, sceneInfoBuffer);
}

function writeNodes(environment, scene, writer, file) {
    const is64Bit = environment.architecture === Environment.Architecture.Arch64;
    const sizeOfVoidPtr = is64Bit ? 8 : 4;
    const transformBuffer = Buffer.allocUnsafe(40);
    const propertiesBuffer = Buffer.allocUnsafe(4 * sizeOfVoidPtr);
    // matrix buffer is currently left empty as it is calculated at runtime
    // this could probably be calulated here and written to file
    const n64MatrixBuffer = Buffer.alloc(64, 0);

    for (const node of scene.nodes) {
        let index = 0;

        // write the transform data
        for (const val of node.position)
            index = writer.writeFloat(transformBuffer, val, index);

        for (const val of node.rotation)
            index = writer.writeFloat(transformBuffer, val, index);

        for (const val of node.scale)
            index = writer.writeFloat(transformBuffer, val, index);

        index = 0

        // write properties data
        // this needs to line up with node.h
        if (is64Bit) {
            index = writer.writeUInt64(propertiesBuffer, node.collider, index);
            index = writer.writeUInt64(propertiesBuffer, node.mesh, index);
            index = writer.writeUInt64(propertiesBuffer, node.data, index);
            index = writer.writeUInt32(propertiesBuffer, node.layerMask, index);
            index = writer.writeUInt32(propertiesBuffer, 0, index); // this is for alignment purposes
        } else {
            index = writer.writeUInt32(propertiesBuffer, node.collider, index);
            index = writer.writeUInt32(propertiesBuffer, node.mesh, index);
            index = writer.writeUInt32(propertiesBuffer, node.data, index);
            index = writer.writeUInt32(propertiesBuffer, node.layerMask, index);
        }

        fs.writeSync(file, transformBuffer);
        fs.writeSync(file, n64MatrixBuffer);
        fs.writeSync(file, propertiesBuffer);
    }
}

function writeCustomBoundingBoxes(scene, writer, file) {
    const boundingBuffer = Buffer.allocUnsafe(Bounding.SizeOf);

    for (const customBoundingBox of scene.customBoundingBoxes) {
        customBoundingBox.write(writer, boundingBuffer, 0);
        fs.writeSync(file, boundingBuffer);
    }
}

/** The write order of this function needs to correspond to in fw64CollisionMesh collider.h */
function writeCollisionMeshes(environment, scene, writer, file) {
    const sizeOfVoidPtr = environment.architecture === Environment.Architecture.Arch64 ? 8 : 4;
    const infoBuffer = Buffer.allocUnsafe(8 + (2 * sizeOfVoidPtr) + Bounding.SizeOf);

    for (const collisionMesh of scene.collisionMeshes) {
        if (collisionMesh.primitives.length > 1) {
            throw new Error(`Collision meshes should have a single primitive: ${collisionMesh.name}`)
        }
        const primitive = collisionMesh.primitives[0];

        let index = 0;
        index = writer.writeUInt32(infoBuffer, primitive.vertices.length, index);
        index = writer.writeUInt32(infoBuffer, primitive.elements.length * 3, index); // primitive elements are stored as triangles, but we want the total index count
        index = primitive.bounding.write(writer, infoBuffer, index);

        // write empty data for pointers - these will be filled in at runtime
        if (environment.architecture == Environment.Architecture.Arch64) {
            index = writer.writeUInt64(infoBuffer, 0, index);
            index = writer.writeUInt64(infoBuffer, 0, index);
        }
        else {
            index = writer.writeUInt32(infoBuffer, 0, index);
            index = writer.writeUInt32(infoBuffer, 0, index);
        }

        const elementDataSize = primitive.elements.length * 3 * 2;
        const vertexDataSize = primitive.vertices.length * 3 * 4;

        const dataBuffer = Buffer.allocUnsafe(elementDataSize + vertexDataSize);
        index = 0;

        for (const vertex of primitive.vertices) {
            for (let i = 0; i < 3; i++) {
                index = writer.writeFloat(dataBuffer, vertex[i], index);
            }
        }

        for (const element of primitive.elements) {
            for (const e of element) {
                index = writer.writeUInt16(dataBuffer, e, index);
            }
        }

        fs.writeSync(file, infoBuffer);
        fs.writeSync(file, dataBuffer);
    }
}

module.exports = {
    writeSceneInfo: writeSceneInfo,
    writeNodes: writeNodes,
    writeCollisionMeshes: writeCollisionMeshes,
    writeCustomBoundingBoxes: writeCustomBoundingBoxes
};
