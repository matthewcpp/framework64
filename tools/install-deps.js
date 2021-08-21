const path = require("path");
const fs = require("fs");
const {spawn} = require('child_process');

const scriptDir = __dirname;
const vcpkgDir = path.resolve(scriptDir, "..", "lib", "vcpkg");

const executableName = process.platform === "win32" ? "vcpkg.exe" : "vcpkg";
const executablePath = path.join(vcpkgDir, executableName);

const bootstrapScriptName = process.platform === "win32" ? "bootstrap-vcpkg.bat" : "bootstrap-vcpkg.sh";
const bootstrapScriptPath = path.join(vcpkgDir, bootstrapScriptName);

const triplets = {"win32": "x64-windows", "linux": "x64-linux", "darwin": "x64-osx"};

const depsJsonPath = path.join(scriptDir, "vcpkg.json");

function spawnPromise(command, args) {
    return new Promise((resolve, reject) => {
        const spawnedProcess = spawn(command, args);

        spawnedProcess.stdout.on("data", (data) => {
           console.log(`${data}`);
        });
        spawnedProcess.stderr.on("data", (data) => {
            console.log(`${data}`);
        });
        spawnedProcess.on("close", (code) => {
            if (code === 0)
                resolve(code);
            else
                reject(code);
        })
    })
}

async function main() {
    if (!fs.existsSync(executablePath)) {
        console.log("Bootstrapping vcpkg");
        switch (process.platform) {
            case "win32":
                await spawnPromise("cmd.exe", ["/c", bootstrapScriptPath]);
                break;

            case "darwin":
                await spawnPromise("zsh", [bootstrapScriptPath]);
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
    await spawnPromise(executablePath, args);
}

if (require.main === module) {
    main();
}
