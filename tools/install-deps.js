const path = require("path");
const fs = require("fs");
const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const scriptDir = __dirname;
const vcpkgDir = path.resolve(scriptDir, "..", "lib", "vcpkg");

const executableName = process.platform === "win32" ? "vcpkg.exe" : "vcpkg";
const executablePath = path.join(vcpkgDir, executableName);

const bootstrapScriptName = process.platform === "win32" ? "bootstrap-vcpkg.bat" : "bootstrap-vcpkg.sh";
const bootstrapScriptPath = path.join(vcpkgDir, bootstrapScriptName);

const triplets = {"win32": "x64-windows", "linux": "x64-linux", "darwin": "x64-osx"};

const depsJsonPath = path.join(scriptDir, "vcpkg.json");

async function main() {
    if (!fs.existsSync(executablePath)) {
        console.log("Bootstrapping vcpkg");
        switch (process.platform) {
            case "win32":
                await execFile("cmd.exe", ["/c", bootstrapScriptPath]);
                break;

            case "darwin":
                await execFile("zsh", [bootstrapScriptPath]);
                break;
        }

    }

    const args = ["install", ...JSON.parse(fs.readFileSync(depsJsonPath, {encoding: "utf8"}))];
    const triplet = triplets[process.platform];

    if (triplet) {
        args.push("--triplet", triplet);
    }
    else {
        console.log(`Warning: Unsupported platform: ${process.platform}`);
    }

    console.log(args);
    console.log("Installing vcpkg packages");
    await execFile(executablePath, args);
}

if (require.main === module) {
    main();
}
