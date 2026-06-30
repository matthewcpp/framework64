const Util = require("../Util");
const path = require("path");
const fs = require("fs");

class LayersProcessor {
    _environment;

    constructor(environment) {
        this._environment = environment;
    }

    process(layersFilePath) {
        if (fs.existsSync(layersFilePath)) {
            console.log(`Processing Layers: ${layersFilePath}`);
            const layersJson = JSON.parse(fs.readFileSync(layersFilePath, {encoding: "utf-8"}));
            return this._processLayerObject(layersJson)
        } else {
            return this._processLayerObject({});
        }
    }

    _processLayerObject(layers) {
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

        this._writeMapping(mapping, path.join(this._environment.includeDirectory, "layers.h"));

        return mapping;
    }

    _writeMapping(mapping, destFile) {
        const file = fs.openSync(destFile, "w");

        fs.writeSync(file, "#pragma once\n\n");

        mapping.forEach((value, key) => {
            fs.writeSync(file, `#define FW64_layer_${key} ${value}U\n`);
        });

        fs.writeSync(file, "\n");
        fs.closeSync(file);
    }
};

module.exports = LayersProcessor;