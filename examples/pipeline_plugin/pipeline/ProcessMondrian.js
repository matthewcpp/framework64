const fs = require("fs");
const path = require("path");

class ProcessRectangles {
    /** 
     * All plugin classes must define this method.  
     * It will be called by the asset pipeline when a file is encountered with the registered extension 
     * @param fileJson this is the json object for the particular file to be processed
     * @param environment this is the Environment object which contains relevant processing information
     * */
    async process(fileJson, environment) {
        // determine the path of the source file on dick and load
        const sourcePath = path.join(environment.assetDirectory, fileJson.src);
        const mondrian = JSON.parse(fs.readFileSync(sourcePath, {encoding: "utf8"}));

        // maximum title length would be 63 characters.  We will add a null terminator
        const titleBuffer = Buffer.alloc(64);
        const titleIndex = titleBuffer.write(mondrian.title, 0);
        titleBuffer.writeUint8(0, titleIndex);

        // each rectangle will consume 20 bytes of data
        const writer = environment.binaryWriter;
        const rectBuffer = Buffer.alloc(4 /*length*/ + mondrian.elements.length * 20);
        let rectBufferIndex = writer.writeUInt32(rectBuffer, mondrian.elements.length, 0);
        for (const element of mondrian.elements) {
            rectBufferIndex = writer.writeUInt32(rectBuffer, element.x, rectBufferIndex);
            rectBufferIndex = writer.writeUInt32(rectBuffer, element.y, rectBufferIndex);
            rectBufferIndex = writer.writeUInt32(rectBuffer, element.width, rectBufferIndex);
            rectBufferIndex = writer.writeUInt32(rectBuffer, element.height, rectBufferIndex);

            rectBufferIndex = writer.writeUInt8(rectBuffer, element.color[0], rectBufferIndex);
            rectBufferIndex = writer.writeUInt8(rectBuffer, element.color[1], rectBufferIndex);
            rectBufferIndex = writer.writeUInt8(rectBuffer, element.color[2], rectBufferIndex);
            rectBufferIndex = writer.writeUInt8(rectBuffer, element.color[3], rectBufferIndex);
        }

        // determine the path of the binary output file and write it
        const destFileName = path.basename(fileJson.src, path.extname(fileJson.src)) + ".mondrian";
        const destPath = path.join(environment.outputDirectory, destFileName);
        let handle = fs.openSync(destPath, "w");
        fs.writeSync(handle, titleBuffer);
        fs.writeSync(handle, rectBuffer);
        fs.closeSync(handle);

        // add an entry for the asset in the file bundle
        environment.assetBundle.addFile(destPath);
    }
}

module.exports = ProcessRectangles;