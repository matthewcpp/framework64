const gltfConvert = require("./asset_pipeline/GLTFConvert");
const imageConvert = require("./asset_pipeline/ImageConvert");
const FontConvert = require("./asset_pipeline/FontConvert");

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

    if (manifest.models) {
        for (const model of manifest.models) {
            const sourceFile = path.join(manifestDirectory, model.src);

            await gltfConvert(sourceFile, outputDirectory, model);
        }
    }

    if (manifest.sprites) {
        for (const sprite of manifest.sprites) {
            const sourceFile = path.join(manifestDirectory, sprite.src);
            await imageConvert.convertSprite(sourceFile, outputDirectory, sprite);
        }
    }

    if (manifest.fonts) {
        for (const font of manifest.fonts) {
            const sourceFile = path.join(manifestDirectory, font.src);
            await FontConvert.convertFont(sourceFile, outputDirectory, font);
        }
    }
}



if (require.main === module) {
    main();
}