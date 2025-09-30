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

/** Returns the first child of the supplied node whose name begins with the supplied string.
 * If no node is found, returns null.
 */
function findChildNodeStartingWith(gltfData, gltfNode, str, ignoreCase = false) {
    if (!Object.hasOwn(gltfNode, "children")) {
        return null;
    }

    if (ignoreCase) {
        str = str.toLowerCase();
    }

    for (const nodeIndex of gltfNode.children) {
        const node = gltfData.gltf.nodes[nodeIndex];

        if (!Object.hasOwn(node, "name")) {
            continue;
        }

        const nodeName = ignoreCase ? node.name.toLowerCase() : node.name;

        if (nodeName.startsWith(str)) {
            return node;
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
    findChildNodeStartingWith: findChildNodeStartingWith,
    getSceneRootNode: getSceneRootNode
}