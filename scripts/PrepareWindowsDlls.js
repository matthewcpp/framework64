const fse = require("fs-extra");
const path = require("path");


async function prepareWindowsDlls(desktopBuildDir, outputDirectory) {
	console.log("Preparing Windows DLL's");
	const buildInfoPath = path.join(desktopBuildDir, "build_info.json");

	if (!fse.existsSync(buildInfoPath)) {
		throw new Error("Unable to locate build_info.json.  Ensure that you have configured a desktop build");
	}

	const buildInfo = JSON.parse(fse.readFileSync(buildInfoPath, {encoding: "utf-8"}))

	if (!process.env.VCPKG_DIR) {
		throw new Error("Please ensure that the VCPKG_DIR environment is set and points to the root directory of vcpkg");
	}

	const vcpkgRoot = process.env.VCPKG_DIR;

	const dllDestDir = outputDirectory;
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
		console.log("Usage: node prepareWindowsDlls.js /path/to/build_desktop /path/to/outdir");
		process.exit(1);
	}

	try {
		prepareWindowsDlls(process.argv[2])
	}
	catch (e) {
		console.error(e);
		process.exit(1);
	}
}
