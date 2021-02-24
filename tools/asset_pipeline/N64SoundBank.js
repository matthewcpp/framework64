const tmp = require("tmp");

const path = require("path");
const fs = require("fs");

class N64SoundBank {
    tempDir = tmp.dirSync({unsafeCleanup: true});
    name;
    fileCount = 0;

    constructor(name) {
        this.name = name;
    }

    load(files) {
        let index = 0;

        try {
            for (const srcFile of files) {
                const ext = path.extname(srcFile)
                const destFile = path.join(this.tempDir.name, `${index++}${ext}`);
                fs.copyFileSync(srcFile, destFile);
            }

            this.fileCount = files.length;
        }
        catch (e) {
            this.cleanup();

            console.log(`Unable to locate sound effect: ${files[index - 1]}`);
        }

    }

    cleanup() {
        this.tempDir.removeCallback();
        this.tempDir = null;
    }

    get directory() {
        return this.tempDir.name;
    }

    get ctlFileName() {
        return this.name + ".ctl";
    }
    
    get ctlFilePath() {
        return path.join(this.directory, this.ctlFileName);
    }

    get tblFileName() {
        return this.name + ".tbl";
    }

    get tblFilePath() {
        return path.join(this.directory, this.tblFileName);
    }
}

module.exports = N64SoundBank;