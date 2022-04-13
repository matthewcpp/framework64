// npm run --prefix n64-sandbox create-game MyGame

const fs = require("fs");
const path = require("path");

const replaceExtensions = new Set([".c", ".txt"]);

if (process.argv.length < 4) {
	console.log("Useage: npm run create-game MyGame /path/to/directory");
	process.exit(1);
}

const gameName = process.argv[2];
const gameDirectory = process.argv[3];

if (!fs.existsSync(gameDirectory)) {
	console.error(`Directory does not exist: ${gameDirectory}`)
	process.exit(1);
}

const submoduleDirectory = path.join(gameDirectory, "lib", "framework64");
if (!fs.existsSync(submoduleDirectory)) {
	console.warn(`Framework64 submodule directory does not exist: ${submoduleDirectory}`)
}

console.log("Assemble package.json file")

const packageJsonSourcePath = path.resolve(__dirname, "..", "package.json");
const packageJsonSource = JSON.parse(fs.readFileSync(packageJsonSourcePath, {encoding: "utf-8"}));


const commonNpmScripts = {
	"prepare-n64-assets": "node lib/framework64/pipeline/Pipeline.js -f assets/manifest.json n64 build_n64/bin/assets",
	"prepare-n64-modernsdk-development": "cp lib/framework64/config/n64.modernsdk/n64.modernsdk.devcontainer.json .devcontainer.json",
	"prepare-desktop-assets": "node lib/framework64/pipeline/Pipeline.js -f assets/manifest.json desktop build_desktop/bin/assets",
	"prepare-desktop-shaders": "node lib/framework64/scripts/PrepareDesktopShaders.js lib/framework64/src/desktop/glsl build_desktop/bin/glsl",
	"prepare-windows-dlls": "node lib/framework64/scripts/PrepareWindowsDlls.js build_desktop",
};

// cp does not work in powershell via 'npm run ...''
const windowsNpmScripts = {
	"prepare-n64-modernsdk-development": "copy lib\\framework64\\config\\n64.modernsdk\\n64.modernsdk.devcontainer.json .devcontainer.json",
}

let npmScripts = commonNpmScripts;

if (process.platform === "win32")
	npmScripts = Object.assign(npmScripts, windowsNpmScripts);

const packageJsonOutput = {
	"name": gameName,
	"scripts": npmScripts,
	"dependencies": packageJsonSource.dependencies
};

const packageJsonOutputPath = path.join(gameDirectory, "package.json");
fs.writeFileSync(packageJsonOutputPath, JSON.stringify(packageJsonOutput, null, 4));

function copyDirSync(sourceDir, destDir) {
	if (!fs.existsSync(destDir)) {
		fs.mkdirSync(destDir);
	}

	const items = fs.readdirSync(sourceDir, {withFileTypes: true});
	
	for (const item of items) {
		const sourcePath = path.join(sourceDir, item.name);
		const destPath = path.join(destDir, item.name);

		if (item.isFile()) {
			if (replaceExtensions.has(path.extname(sourcePath))) {
				const fileText = fs.readFileSync(sourcePath, {encoding: "utf-8"});
				const replacedText = fileText.replaceAll("minimal", gameName);
				fs.writeFileSync(destPath, replacedText);
			}
			else {
				fs.copyFileSync(sourcePath, destPath);
			}
		}
		else if (item.isDirectory()){
			copyDirSync(sourcePath, destPath);
		}
	}
}

const newProjectDir = path.resolve(__dirname, "..", "config", "new_project");
console.log(`Copy project starter files: ${newProjectDir} -> ${gameDirectory}`);
copyDirSync(newProjectDir, gameDirectory);

const srcDir = path.join(gameDirectory, "src");
const minimalDir = path.resolve(__dirname, "..", "examples", "minimal");

console.log(`Copy minimal code: ${minimalDir} -> ${srcDir}`);
copyDirSync(minimalDir, srcDir);

const vsCodeSrcDir = path.resolve(__dirname, "..", ".vscode");
const vsCodeDestDir = path.resolve(gameDirectory, ".vscode");
console.log(`Copy VSCode files ${vsCodeSrcDir} -> ${vsCodeDestDir}`);
copyDirSync(vsCodeSrcDir, vsCodeDestDir);