const gltfConvert = require("./asset_pipeline/GLTFConvert");
const pngConvert = require("./asset_pipeline/PNGConvert");
const fs = require("fs");
const path = require("path");
const rimraf = require("rimraf");

async function main() {
    const manifestFile = path.resolve(__dirname, "..", "assets", "manifest.n64.json");
    const manifestDirectory = path.dirname(manifestFile);

    const manifest = JSON.parse(fs.readFileSync(manifestFile, "utf8"));

    const outputDirectory = path.resolve(path.dirname(manifestFile), manifest.outDir);

    if (fs.existsSync(outputDirectory)) {
        rimraf.sync(outputDirectory);
    }

    fs.mkdirSync(outputDirectory);

    console.log(manifest);
    console.log(outputDirectory);

    for (const model of manifest.models) {
        const sourceFile = path.join(manifestDirectory, model.src);

        await gltfConvert(sourceFile, outputDirectory, model);
    }

    for (const image of manifest.images) {
        const sourceFile = path.join(manifestDirectory, image.src);
        const fileName = path.basename(image.src, ".png") + ".h";
        const outputFile = path.join(outputDirectory, fileName);

        await pngConvert(sourceFile, outputFile);
    }
}



if (require.main === module) {
    main();
}