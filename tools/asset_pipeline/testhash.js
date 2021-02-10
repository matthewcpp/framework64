const UlongMax = BigInt(0xFFFFFFFF);
const ULongWrap = UlongMax + 1n;

function str_hash(str) {
    let hash = BigInt(5381);
    for (let i = 0; i < str.length; i++) {
        const c = BigInt(str.charCodeAt(i));

        let h = (hash << 5n) & UlongMax;
        h = (h + hash) % ULongWrap;
        hash = (h + c) % ULongWrap;
    }

    return parseInt(hash);
}

function print_hash(str) {
    console.log(str, str_hash(str));
}


print_hash("ken.png");
print_hash("nintendo_seal.png");
print_hash("n64_logo.png");
print_hash("n64_logo_model/n64_logo.gltf");
print_hash("suzanne/suzanne.gltf");
print_hash("penguin/penguin.gltf");
print_hash("blue_cube/blue_cube.gltf");
print_hash("Consolas.ttf");
print_hash("basicLAZER.otf");
print_hash("sample.txt");
