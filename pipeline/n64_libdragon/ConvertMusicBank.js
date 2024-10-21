const fs = require("fs");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const inputDirectory = "/music_bank";
const outputDirectory = "/dfs_assets";
const autoconv64 = "/opt/libdragon/bin/audioconv64"

const files = fs.readdirSync(inputDirectory);

for (const file of files) {
    
}