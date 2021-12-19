const fse = require("fs-extra");
const path = require("path");


async function prepareWindowsDlls(desktopBuildDir) {
	const buildInfoPath = path.join(desktopBuildDir, "build_info.json");

	if (!fse.existsSync(buildInfoPath)) {
		console.log("Unable to locate build_info.json.  Ensure that you have configured a desktop build");
		process.exit(1);
	}

	const buildInfo = JSON.parse(fse.readFileSync(buildInfoPath, {encoding: "utf-8"}))

	if (!process.env.VCPKG_DIR) {
		console.log("Please ensure that the VCPKG_DIR environment is set and points to the root directory of vcpkg");
		process.exit(1);
	}

	const vcpkgRoot = process.env.VCPKG_DIR;

	const dllDestDir = path.join(desktopBuildDir, "bin");
	await fse.ensureDir(dllDestDir);

	const arch = buildInfo.target.indexOf("x86") >= 0 ? "x86" : "x64";
	const buildType = buildInfo.type === "Debug" ? "dbg" : "rel";

	const vorbisRoot = path.join(vcpkgRoot, `buildtrees/libvorbis/${arch}-windows-${buildType}/lib`);
	const vorbisDlls = ["vorbis.dll", "vorbisfile.dll"];
	
	for (const vorbisDll of vorbisDlls) {
		const dllSrc = path.join(vorbisRoot, vorbisDll);
		const dllDest = path.join(dllDestDir, vorbisDll);
		fse.copySync(dllSrc, dllDest);
		console.log(`Copy ${dllSrc} --> ${dllDest}`)
	}

	const oggRootPath = path.join(vcpkgRoot, `buildtrees/libogg/${arch}-windows-${buildType}`);
	const oggSrc = path.join(oggRootPath, "ogg.dll");
	const oggDest = path.join(dllDestDir, "ogg.dll");
	fse.copySync(oggSrc, oggDest);
	console.log(`Copy ${oggSrc} --> ${oggDest}`)
}

module.exports = prepareWindowsDlls;


if (require.main === module) {
	if (process.platform != "win32") {
		console.log("This script is only required on windows");
		process.exit(1);
	}

	if (process.argv.length < 3) {
		console.log("Usage: node prepareWindowsDlls.js C:/path/to/framework64/build_desktop");
		process.exit(1);
	}

	prepareWindowsDlls(process.argv[2])
}
