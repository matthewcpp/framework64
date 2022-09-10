const fs = require("fs");
const path = require("path");
const crypto = require('crypto');

class ArchiveEntry {
    path
    type
    index
    assetId

    constructor(assetPath, type, index) {
        this.path = assetPath;
        this.type = type;
        this.index = index;

        const name = path.basename(assetPath, path.extname(assetPath));
        this.assetId = `FW64_ASSET_${type}_${name}`;
    }
}

class Archive {
    entries = new Map();
    entryHashes = new Map();
    fileName = "assets"
    outputDirectory;
    includeDirectory;

    constructor(outputDirectory, includeDirectory) {
        this.outputDirectory = outputDirectory;
        this.includeDirectory = includeDirectory;
    }

    async add(path, type) {
        if (this.entries.has(path))
            throw new Error(`Duplicate key in archive: ${path}`);

        if (!fs.existsSync(path)) {
            throw new Error(`Compiled asset path: ${path} does not exist.  This is most likely an error in the asset pipeline itself.`);
        }

        const hash = await this._hashFile(path);

        let entry = this.entryHashes.get(hash);
        if (entry) {
            return entry;
        }

        const index = this.entries.size;
        entry = new ArchiveEntry(path, type, index);
        this.entries.set(path, entry);
        this.entryHashes.set(hash, entry);

        return entry;
    }

    async _hashFile(path) {
        return new Promise((resolve, reject) => {
            const hash = crypto.createHash('sha1')
            const rs = fs.createReadStream(path)
            rs.on('error', reject)
            rs.on('data', chunk => hash.update(chunk))
            rs.on('end', () => resolve(hash.digest('hex')))
        })
    }

    write() {
        const headerPath = path.join(this.includeDirectory, `${this.fileName}.h`);
        const archivePath = path.join(this.outputDirectory, `${this.fileName}.dat`);
        const manifestPath = path.join(this.outputDirectory, `${this.fileName}.manifest.json`);

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
        fs.writeSync(header, "#pragma once\n");

        fs.writeSync(header, `#define FW64_ASSET_COUNT ${this.entries.size}\n\n`);

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
            fs.writeSync(header, `#define ${entry.assetId} ${entry.index}\n`);

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

        fs.writeSync(archive, headerBuffer);
    }
}


module.exports = Archive;