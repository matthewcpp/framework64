const fs = require("fs");
const path = require("path");

class ArchiveEntry {
    path
    type
    index

    constructor(path, type, index) {
        this.path = path;
        this.type = type;
        this.index = index;
    }
}

class Archive {
    entries = new Map();
    fileName = "assets"

    add(path, type) {
        const index = this.entries.size;
        const entry = new ArchiveEntry(path, type, index);
        this.entries.set(path, entry);

        return entry;
    }

    write(outputDir) {
        const headerPath = path.join(outputDir, `${this.fileName}.h`);
        const archivePath = path.join(outputDir, `${this.fileName}.dat`);
        const manifestPath = path.join(outputDir, `${this.fileName}.manifest.json`);

        const header = fs.openSync(headerPath, 'w');
        const archive = fs.openSync(archivePath, 'w');

        const manifest = {
            files: []
        };

        this._write(header, archive, manifest);

        fs.closeSync(header);
        fs.closeSync(archive);

        fs.writeFileSync(manifestPath, JSON.stringify(manifest, null, 2), {encoding: "utf8"});
    }

    _write(header, archive, manifest) {
        fs.writeSync(header, "#ifndef ASSETS_H\n");
        fs.writeSync(header, "#define ASSETS_H\n\n");

        fs.writeSync(header, `#define ASSET_COUNT ${this.entries.size}\n\n`);

        // setup the header which will be placed at the beginning of the archive
        const headerBuffer = Buffer.alloc(4 + (this.entries.size * 4));
        let headerBufferOffset = headerBuffer.writeUInt32BE(this.entries.size);
        fs.writeSync(archive, headerBuffer); // reserve header size in file

        const binaryBuffer = Buffer.alloc(4); //used as a scratch for binary data

        let archiveOffset = headerBuffer.length;
        this.entries.forEach((entry) => {
            // record offset for the current item in the archive header buffer
            headerBufferOffset = headerBuffer.writeUInt32BE(archiveOffset, headerBufferOffset);

            // define the asset index in the header file
            const name = path.basename(entry.path, path.extname(entry.path));
            fs.writeSync(header, `#define ASSET_${entry.type}_${name} ${entry.index}\n`);

            const data = fs.readFileSync(entry.path);

            binaryBuffer.writeUInt32BE(data.length);
            fs.writeSync(archive, binaryBuffer);
            fs.writeSync(archive, data);

            manifest.files.push({
                path: entry.path,
                offset: archiveOffset,
                size: data.length
            })

            archiveOffset += 4 + data.length;

            //each file should be 2 byte aligned to help with reads
            if ((archiveOffset & 0x1) !== 0) {
                binaryBuffer.writeUInt8(0);
                fs.writeSync(archive, binaryBuffer, 0, 1);
                archiveOffset += 1;
            }

        })

        //write the completed header into the archive
        fs.writeSync(archive, headerBuffer, 0, headerBuffer.length, 0);

        fs.writeSync(header, `#endif\n`);
        fs.writeSync(archive, headerBuffer);
    }
}


module.exports = Archive;