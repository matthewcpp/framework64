const gltfConvert = require("./asset_pipeline/GLTFConvert");
const imageConvert = require("./asset_pipeline/ImageConvert");
const FontConvert = require("./asset_pipeline/FontConvert");
const Archive = require("./asset_pipeline/Archive");

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

    const archive = new Archive();

    fs.mkdirSync(outputDirectory);

    if (manifest.models) {
        for (const model of manifest.models) {
            const sourceFile = path.join(manifestDirectory, model.src);

            await gltfConvert(sourceFile, outputDirectory, model);
        }
    }

    if (manifest.sprites) {
        for (const sprite of manifest.sprites) {
            if (sprite.src) {
                const sourceFile = path.join(manifestDirectory, sprite.src);
                await imageConvert.convertSprite(sourceFile, outputDirectory, sprite, archive);
            }
            else if (sprite.frames){
                await imageConvert.assembleSprite(manifestDirectory, outputDirectory, sprite, archive);
            }
            else {
                throw new Error("Sprite element must specify 'src' or 'frames'");
            }
        }
    }

    if (manifest.fonts) {
        for (const font of manifest.fonts) {
            const sourceFile = path.join(manifestDirectory, font.src);
            await FontConvert.convertFont(sourceFile, outputDirectory, font, archive);
        }
    }

    if (manifest.raw) {
        for (const item of manifest.raw) {
            const sourceFile = path.join(manifestDirectory, item);
            archive.add(sourceFile, "raw");
        }
    }

    archive.write(outputDirectory);
}

if (require.main === module) {
    main();
}