// npm run --prefix n64-sandbox create-game MyGame

const fs = require("fs");
const path = require("path");

function mainFunc() {
	if (process.argv.length < 4) {
		console.log("Useage: npm run create-game MyGame /path/to/directory");
		process.exit(1);
	}

	const gameName = process.argv[2];
	const gameDirectory = process.argv[3];

	configureNewGame(gameDirectory, gameName);
}

function configureNewGame(gameDirectory, gameName) {
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

	const commonNpmScriptsPath = path.resolve(__dirname, "..", "config", "new_game", "npm_scripts.json");
	const platformNpmScriptsPath = path.resolve(__dirname, "..", "config", "new_game", `npm_scripts.${process.platform}.json`);

	let npmScripts = JSON.parse(fs.readFileSync(commonNpmScriptsPath, "utf-8"));

	if (fs.existsSync(platformNpmScriptsPath)) {
		const platformNpmScripts = JSON.parse(fs.readFileSync(platformNpmScriptsPath, "utf-8"));
		npmScripts = Object.assign(npmScripts, platformNpmScripts);
	}

	const packageJsonOutput = {
		"name": gameName,
		"scripts": npmScripts,
		"dependencies": packageJsonSource.dependencies
	};

	const packageJsonOutputPath = path.join(gameDirectory, "package.json");
	fs.writeFileSync(packageJsonOutputPath, JSON.stringify(packageJsonOutput, null, 4));

	const newProjectDir = path.resolve(__dirname, "..", "config", "new_game", "directory");
	console.log(`Copy game starter files: ${newProjectDir} -> ${gameDirectory}`);
	copyDirSync(newProjectDir, gameDirectory, new Set([".c", ".json", ".txt"]), "minimal", gameName);

	const srcDir = path.join(gameDirectory, "src");
	const minimalDir = path.resolve(__dirname, "..", "examples", "minimal");

	console.log(`Copy minimal code: ${minimalDir} -> ${srcDir}`);
	copyDirSync(minimalDir, srcDir, new Set([".c", ".txt"]), "minimal", gameName);
	}

function copyDirSync(sourceDir, destDir, replaceExtensions, target, replace) {
	if (!fs.existsSync(destDir)) {
		fs.mkdirSync(destDir);
	}

	const items = fs.readdirSync(sourceDir, {withFileTypes: true});
	
	for (const item of items) {
		const sourcePath = path.join(sourceDir, item.name);
		const destPath = path.join(destDir, item.name);

		if (item.isFile()) {
			if (replaceExtensions != null && replaceExtensions.has(path.extname(sourcePath))) {
				const fileText = fs.readFileSync(sourcePath, {encoding: "utf-8"});
				const replacedText = fileText.replaceAll(target, replace);
				fs.writeFileSync(destPath, replacedText);
			}
			else {
				fs.copyFileSync(sourcePath, destPath);
			}
		}
		else if (item.isDirectory()){
			copyDirSync(sourcePath, destPath, replaceExtensions, target, replace);
		}
	}
}


if (require.main === module) {
    mainFunc();
}