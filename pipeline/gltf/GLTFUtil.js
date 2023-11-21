/** Gets the node that represents the scene root node for all static geometry in the scene */
function getSceneRootNode(gltf, rootNode) {
    if (!rootNode.children)
        return null;

    // a node is a scene root node if it has a child that begins with Scene
    for (const childIndex of rootNode.children) {
        const childNode = gltf.nodes[childIndex];

        if (!childNode.name)
            continue;

        if (childNode.name.startsWith("Scene")) {
            return childNode;
        }
    }

    return null;
}

/** Gets the top level nodes that represent discreet scenes in a level. */
function extractTopLevelSceneNodeIndices(gltf) {
    const sceneNode = gltf.scenes[gltf.scene];

    const scenes = [];

    for (const rootNodeIndex of sceneNode.nodes) {
        const topLevelNode = gltf.nodes[rootNodeIndex];

        if (getSceneRootNode(gltf, topLevelNode))
            scenes.push(rootNodeIndex);
    }

    return scenes;
}

module.exports = {
    extractTopLevelSceneNodeIndices: extractTopLevelSceneNodeIndices,
    getSceneRootNode: getSceneRootNode
}