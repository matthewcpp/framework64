const fs = require("fs");
const path = require("path");

function makeSafeName(name) {
    return name.replaceAll(' ', '_').replaceAll('-', '_');
}

function findSceneNode(gltf) {
    for (const node of gltf.nodes) {
        if (node.name === "Scene")
            return node;
    }
    return null;
}

function writeSceneDefines(gltf, name, destFile) {
    name = makeSafeName(name);
    const file = fs.openSync(destFile, "w");

    fs.writeSync(file, "#pragma once\n\n");

    const sceneNode = findSceneNode(gltf);
    if (sceneNode == null) {
        throw new Error(`Unable to locate scene root node in scene: ${name}`);
    }

    if (sceneNode.children) {
        for (let i = 0; i < sceneNode.children.length; i++) {
            const childIndex = sceneNode.children[i];
            const node = gltf.nodes[childIndex];

            if (!node.hasOwnProperty("name") || !node.hasOwnProperty("extras"))
                continue;

            if (!node.extras.type)
                continue;

            const nodeName = makeSafeName(node.name);
            fs.writeSync(file, `#define FW64_scene_${name}_node_${nodeName} ${i}\n`);
        }
    }

    fs.writeSync(file, "\n");
    fs.closeSync(file);
}

module.exports = {
    writeToFile: writeSceneDefines
}