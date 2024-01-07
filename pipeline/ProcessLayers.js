const Util = require("./Util");
const path = require("path");
const fs = require("fs");

function processLayers(manifestDirectory, outputDirectory) {
    const layersFilePath = path.join(manifestDirectory, "layers.json");

    if (fs.existsSync(layersFilePath)) {
        console.log(`Processing Layers: layers.json`);
        const layersJson = JSON.parse(fs.readFileSync(layersFilePath, {encoding: "utf-8"}));
        return _processLayerObject(layersJson, outputDirectory)
    } else {
        return _processLayerObject({}, outputDirectory);
    }
}

function _processLayerObject(layers, outputDirectory) {
    var mapping = new Map();

    var keys = Object.keys(layers);
    for (const key of keys) {
        const safeName = Util.safeDefineName(key);
        const value = parseInt(layers[key]);

        if (key !== safeName)
            throw new Error(`Layer parsing error: ${key}: Layer names should not spaces or special characters.`);

        if (value < 0 || value > 30) {
            throw new Error(`Error parsing layers: ${key}: value should be an integer in range [0, 30]`);
        }

        if (mapping.has(key)) {
            throw new Error(`Error parsing layers: ${key}: Duplicate value found`);
        }

        mapping.set(key, 1 << value);
    }

    writeMapping(mapping, path.join(outputDirectory, "layers.h"));

    return mapping;
}

function writeMapping(mapping, destFile) {
    const file = fs.openSync(destFile, "w");

    fs.writeSync(file, "#pragma once\n\n");

    mapping.forEach((value, key) => {
        fs.writeSync(file, `#define FW64_layer_${key} ${value}\n`);
    });

    fs.writeSync(file, "\n");
    fs.closeSync(file);
}

module.exports = processLayers;