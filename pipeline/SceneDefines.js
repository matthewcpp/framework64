const Util = require("./Util");

const fs = require("fs");

function writeSceneDefines(scene, destFile) {
    const file = fs.openSync(destFile, "w");

    fs.writeSync(file, "#pragma once\n\n");
    const sceneName = Util.safeDefineName(scene.name);


    for (let i = 0; i < scene.nodes.length; i++) {
        const node = scene.nodes[i];

        if (node.hasOwnProperty("name")){
            const nodeName = Util.safeDefineName(node.name);
            fs.writeSync(file, `#define FW64_scene_${sceneName}_node_${nodeName} ${i}\n`);
        }
    }

    fs.writeSync(file, "\n");
    fs.closeSync(file);
}

module.exports = {
    writeToFile: writeSceneDefines
}