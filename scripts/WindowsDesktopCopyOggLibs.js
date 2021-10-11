const path = require("path");
const fse = require("fs-extra");

if (!process.env.VCPKG_DIR) {
    console.log("Error: Environment Variable not set.");
    console.log("This Script requires the VCPKG_DIR directory be set to the root directory of your vcpkg repo");
    process.exit(1);
}

if (process.argv.length < 4) {
    console.log("Error: Invalid argument length");
    console.log("You must specify a build type and architecture");
    process.exit(1);
}

const vcpkgDir = process.env.VCPKG_DIR;
const buildType = process.argv[2];
const arch = process.argv[3];

const destDir = path.resolve("build_desktop/bin");
fse.ensureDirSync(destDir);

const dlls = [
    {
        src: path.join(vcpkgDir, `buildtrees/libogg/${arch}-windows-${buildType}/ogg.dll`),
        dest: path.join(destDir, "ogg.dll")
    },
    {
        src: path.join(vcpkgDir, `buildtrees/libvorbis/${arch}-windows-${buildType}/lib/vorbis.dll`),
        dest: path.join(destDir, "vorbis.dll")
    },
    {
        src: path.join(vcpkgDir, `buildtrees/libvorbis/${arch}-windows-${buildType}/lib/vorbisfile.dll`),
        dest: path.join(destDir, "vorbisfile.dll")
    }
];


for (const dll of dlls) {
    if (!fse.existsSync(dll.src)) {
        console.log(`Unable to locate ${dll.src}.  Have you run Cmake Configure?`);
        process.exit(1);
    }

    console.log(`${dll.src} -> ${dll.dest}`);
    fse.copyFileSync(dll.src, dll.dest);
}
