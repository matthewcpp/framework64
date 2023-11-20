class SceneInfo {
    nodeCount;
    meshCount;
    colliderCount;
    collisionMeshCount;

    constructor(scene) {
        this.nodeCount = scene.nodes.length;
        this.meshCount = scene.meshes.length;
        this.colliderCount = scene.colliderCount;
        this.collisionMeshCount = scene.collisionMeshes.length;
        this.customBoundingBoxCount = scene.customBoundingBoxes.length;
    }

    getBuffer(writer) {
        const buff = Buffer.alloc(20);
        let index = 0;

        index = writer.writeUInt32(buffer, this.nodeCount, index);
        index = writer.writeUInt32(buffer, this.meshCount, index);
        index = writer.writeUInt32(buffer, this.colliderCount, index);
        index = writer.writeUInt32(buffer, this.collisionMeshCount, index);
        index = writer.writeUInt32(buffer, this.customBoundingBoxCount, index);

        return buff;
    }
}

module.exports = SceneInfo;
