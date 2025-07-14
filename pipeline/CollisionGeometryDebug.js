const GLTFLoader = require("./gltf/GLTFLoader");

const fs = require("fs");
const Material = require("./gltf/Material");
const Primitive = require("./gltf/Primitive");
const Scene = require("./gltf/Scene");
const LevelParser = require("./LevelParser");
const MaterialBundle = require("./gltf/MaterialBundle");
const Mesh = require("./gltf/Mesh");

function writeTextFile(collisionGeometry, filePath) {
    const file = fs.openSync(filePath, "w");

    fs.writeSync(file, `cells: ${collisionGeometry.cells.length} triangles: ${collisionGeometry.triangleCount}\n`);

    for (const cell of collisionGeometry.cells) {
        fs.writeSync(file, "    floors:\n")
        for (const t of cell.floors) {
            fs.writeSync(file, `        a: [${t[0][0]}, ${t[0][1]}, ${t[0][2]}] b: [${t[1][0]}, ${t[1][1]}, ${t[1][2]}] c: [${t[2][0]}, ${t[2][1]}, ${t[2][2]}]\n`);
        }
    }

    fs.closeSync(file);
}

function _createTrianglePos(point, color) {
    return [point[0], point[1], point[2],
            0, /* unused flag*/
            0, 0, /* texcords */
            ...color /* color or normal */
    ];
}

function createWireScene(collisionGeometry, namePrefix) {
    const gltfLoader = new GLTFLoader();

    // create the materials that will be assigned to the wireframe geometry
    const floorMaterial = new Material();
    floorMaterial.shadingMode = Material.ShadingMode.Line;
    gltfLoader.materials.push(floorMaterial);

    const levelParser = new LevelParser();
    levelParser.gltfData = gltfLoader;

    // create the scene and set up it's material bundle.
    const wireframeScene = new Scene();
    wireframeScene.name = `${namePrefix}_collision_wireframe`;
    wireframeScene.materialBundle = new MaterialBundle(gltfLoader);
    wireframeScene.materialBundle.materials.push(0);
    levelParser.scenes.push(wireframeScene);

    // all cells will be a child of this node for consistency with other scenes.
    const wireframeRootNode = LevelParser.createAndAddNode(wireframeScene, null);
    wireframeRootNode.name = "root";

    let cellIndex = 0;
    for (const cell of collisionGeometry.cells) {
        // create the cell node
        const cellNode = LevelParser.createAndAddNode(wireframeScene, wireframeRootNode);
        cellNode.name = `${namePrefix}_collision_wire_${cellIndex}`;

        // create the mesh which holds the triangles for the collision geometry in this cell
        // each mesh will only have 1 primitive in this case
        const cellMesh = new Mesh("cell");
        const prim = new Primitive(Primitive.ElementType.Lines);
        
        prim.hasPositions = true;
        prim.hasVertexColors = true;
        prim.material = 0;
        cellMesh.primitives.push(prim);

        let color = [0.0, 0.0, 0.8, 1.0];
        for (const triangle of cell.floors) {
            const a = _createTrianglePos(triangle[0], color);
            const ai = prim.vertices.length;
            prim.vertices.push(a);
            prim.bounding.encapsulatePoint(a);

            const b = _createTrianglePos(triangle[1], color);
            const bi = prim.vertices.length;
            prim.vertices.push(b);
            prim.bounding.encapsulatePoint(b);

            const c = _createTrianglePos(triangle[2], color);
            const ci = prim.vertices.length;
            prim.vertices.push(c);
            prim.bounding.encapsulatePoint(c);

            prim.elements.push([ai, bi]);
            prim.elements.push([bi, ci]);
            prim.elements.push([ci, ai]);
        }

        // bundle the mesh in the scene
        wireframeScene.meshBundle.push(gltfLoader.meshes.length)

        // assign the mesh and reserve a mesh instance for it
        cellNode.mesh = gltfLoader.meshes.length
        wireframeScene.meshInstanceCount += 1;
        gltfLoader.meshes.push(cellMesh);

        cellIndex += 1;
    }

    LevelParser.assignNodeChildPointers(wireframeScene);

    return levelParser;
}

module.exports = {
    writeTextFile: writeTextFile,
    createWireScene: createWireScene
}