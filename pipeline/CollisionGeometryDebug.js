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

    fs.writeSync(file, `cells: ${collisionGeometry.cells.length}\n`);
    fs.writeSync(file, `triangle array size: ${collisionGeometry.triangleCount}\n`);
    fs.writeSync(file, `unique triangles: ${collisionGeometry.numUniqueTriangles}\n`);
    fs.writeSync(file, `single cell triangles: ${collisionGeometry.numSingleCellTriangles}\n`);
    fs.writeSync(file, `multi cell triangles: ${collisionGeometry.numMultiCellTriangles}\n`);
    fs.writeSync(file, "\n\n\n");

    for (const cell of collisionGeometry.cells) {
        fs.writeSync(file, `cell ${cell.posX},${cell.posZ}\n`);
        fs.writeSync(file, `    min: [x: ${cell.boundingBox.min[0]}, z: ${cell.boundingBox.min[2]}] max [x: ${cell.boundingBox.max[0]}, z: ${cell.boundingBox.max[2]}]\n`);
        fs.writeSync(file, `    floors: ${cell.floors.length} walls: ${cell.walls.length} ceilings: ${cell.ceilings.length}\n`);
        fs.writeSync(file, "\n");
    }

    fs.closeSync(file);
}

function _createVertexPos(point, color) {
    return [point[0], point[1], point[2],
            0, /* unused flag*/
            0, 0, /* texcords */
            ...color /* color or normal */
    ];
}
function _createTriangleWireframesInPrim(prim, triangles, color) {
    for (const triangle of triangles) {
        const a = _createVertexPos(triangle[0], color);
        const ai = prim.vertices.length;
        prim.vertices.push(a);
        prim.bounding.encapsulatePoint(a);

        const b = _createVertexPos(triangle[1], color);
        const bi = prim.vertices.length;
        prim.vertices.push(b);
        prim.bounding.encapsulatePoint(b);

        const c = _createVertexPos(triangle[2], color);
        const ci = prim.vertices.length;
        prim.vertices.push(c);
        prim.bounding.encapsulatePoint(c);

        prim.elements.push([ai, bi]);
        prim.elements.push([bi, ci]);
        prim.elements.push([ci, ai]);
    }    
}

function _createCellBoundingWireframe(prim, geometry, cell) {
    const boundingColor = [1.0, 1.0, 1.0, 1.0];

    const t000 = prim.vertices.length;
    const p000 = [cell.boundingBox.min[0], geometry.boundingBox.min[1], cell.boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p000);
    prim.bounding.encapsulatePoint(p000);

    const t100 = prim.vertices.length;
    const p100 = [cell.boundingBox.max[0], geometry.boundingBox.min[1], cell.boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p100);
    prim.bounding.encapsulatePoint(p100);

    const t101 = prim.vertices.length;
    const p101 = [cell.boundingBox.max[0], geometry.boundingBox.min[1], cell.boundingBox.max[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p101);
    prim.bounding.encapsulatePoint(p101);

    const t001 = prim.vertices.length;
    const p001 = [cell.boundingBox.min[0], geometry.boundingBox.min[1], cell.boundingBox.max[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p001);
    prim.bounding.encapsulatePoint(p001);

    const t010 = prim.vertices.length;
    const p010 = [cell.boundingBox.min[0], geometry.boundingBox.max[1], cell.boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p010);
    prim.bounding.encapsulatePoint(p010);

    const t110 = prim.vertices.length;
    const p110 = [cell.boundingBox.max[0], geometry.boundingBox.max[1], cell.boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p110);
    prim.bounding.encapsulatePoint(p110);

    const t111 = prim.vertices.length;
    const p111 = [cell.boundingBox.max[0], geometry.boundingBox.max[1], cell.boundingBox.max[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p111);
    prim.bounding.encapsulatePoint(p111);

    const t011 = prim.vertices.length;
    const p011 = [cell.boundingBox.min[0], geometry.boundingBox.max[1], cell.boundingBox.max[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p011);
    prim.bounding.encapsulatePoint(p011);

    prim.elements.push([t000, t100]);
    prim.elements.push([t100, t101]);
    prim.elements.push([t101, t001]);
    prim.elements.push([t001, t000]);

    prim.elements.push([t010, t110]);
    prim.elements.push([t110, t111]);
    prim.elements.push([t111, t011]);
    prim.elements.push([t011, t010]);

    prim.elements.push([t000, t010]);
    prim.elements.push([t100, t110]);
    prim.elements.push([t101, t111]);
    prim.elements.push([t001, t011]);
}

/** This needs to be kept in sync with fw64CollisionSceneManagerLayers in components/fw64_collision_scene_manager.h */
const CollisionDebugLayers = {
    DebugSceneWireTriangleLayer: 1,
    DebugSceneWireGridLayer: 2
};

function _createWireMeshNode(gltfLoader, scene, parent, nodeName){
    // create the cell node
    const cellNode = LevelParser.createAndAddNode(scene, parent);
    cellNode.name = nodeName;

    // create the mesh which holds the triangles for the collision geometry in this cell
    // each mesh will only have 1 primitive in this case
    const cellMesh = new Mesh(nodeName + "_mesh");
    const prim = new Primitive(Primitive.ElementType.Lines);
    
    prim.hasPositions = true;
    prim.hasVertexColors = true;
    prim.material = 0;
    cellMesh.primitives.push(prim);

    // bundle the mesh in the scene
    scene.meshBundle.push(gltfLoader.meshes.length)

    // assign the mesh and reserve a mesh instance for it
    cellNode.mesh = gltfLoader.meshes.length
    scene.meshInstanceCount += 1;
    gltfLoader.meshes.push(cellMesh);

    return [cellNode, prim];
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

    const sceneRootNode = LevelParser.createAndAddNode(wireframeScene, null);
    sceneRootNode.name = "root";

    // all cells will be a child of this node for consistency with other scenes.
    const wireframeRootNode = LevelParser.createAndAddNode(wireframeScene, sceneRootNode);
    wireframeRootNode.name = "wireframe triangles";

    for (const cell of collisionGeometry.cells) {
        const [wireNode, wirePrim] = _createWireMeshNode(gltfLoader, wireframeScene, wireframeRootNode, `${namePrefix}_${cell.posX}_${cell.posZ}_wire_triangles`);
        _createTriangleWireframesInPrim(wirePrim, cell.floors, [0.0, 0.0, 0.8, 1.0]);
        wireNode.layerMask = CollisionDebugLayers.DebugSceneWireTriangleLayer;
    }

    const gridRootNode = LevelParser.createAndAddNode(wireframeScene, sceneRootNode);
    gridRootNode.name = "grid";

    for (const cell of collisionGeometry.cells) {
        const [gridNode, gridPrim] = _createWireMeshNode(gltfLoader, wireframeScene, gridRootNode, `${namePrefix}_${cell.posX}_${cell.posZ}_grid`);
        _createCellBoundingWireframe(gridPrim, collisionGeometry, cell);
        gridNode.layerMask = CollisionDebugLayers.DebugSceneWireGridLayer;
    }

    LevelParser.assignNodeChildPointers(wireframeScene);

    return levelParser;
}

module.exports = {
    writeTextFile: writeTextFile,
    createWireScene: createWireScene
}