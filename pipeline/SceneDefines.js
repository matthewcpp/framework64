const Util = require("./Util");

const fs = require("fs");
const path = require("path");

function writeSceneDefines(gltf, name, sceneNode, destFile) {
    const file = fs.openSync(destFile, "w");

    fs.writeSync(file, "#pragma once\n\n");

    if (sceneNode.children) {
        for (let i = 0; i < sceneNode.children.length; i++) {
            const childIndex = sceneNode.children[i];
            const node = gltf.nodes[childIndex];

            if (!node.hasOwnProperty("name"))
                continue;

            const nodeName = Util.safeDefineName(node.name);
            fs.writeSync(file, `#define FW64_scene_${name}_node_${nodeName} ${i}\n`);
        }
    }

    fs.writeSync(file, "\n");
    fs.closeSync(file);
}

module.exports = {
    writeToFile: writeSceneDefines
}