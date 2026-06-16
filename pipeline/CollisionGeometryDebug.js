const GLTFLoader = require("./gltf/GLTFLoader");

const fs = require("fs");
const Material = require("./gltf/Material");
const Primitive = require("./gltf/Primitive");
const Scene = require("./gltf/Scene");
const LevelParser = require("./LevelParser");
const MaterialBundle = require("./gltf/MaterialBundle");
const Mesh = require("./gltf/Mesh");
const Node = require("./gltf/Node");

/** This should be kept in sync with CollisionGeometryDebugLayerMask in fw64_collision_geometry_debug.h */
const LayerMask = {
    CellBoundings: 1 << 25,
    BoundingVolumes: 1 << 26,
    FloorTriangles: 1 << 27,
    WallTriangles: 1 << 28,
    CeilingTriangles: 1 << 29
};

function writeTextFile(collisionGeometry, filePath) {
    const file = fs.openSync(filePath, "w");

    fs.writeSync(file, `bounding min: [x: ${collisionGeometry.boundingBox.min[0]}, y: ${collisionGeometry.boundingBox.min[1]} z: ${collisionGeometry.boundingBox.min[2]}] max [x: ${collisionGeometry.boundingBox.max[0]}, y: ${collisionGeometry.boundingBox.max[1]}, z: ${collisionGeometry.boundingBox.max[2]}]\n`);
    fs.writeSync(file, `cells: ${collisionGeometry.cells.length}\n`);
    fs.writeSync(file, `triangle array size: ${collisionGeometry.triangleCount}\n`);
    fs.writeSync(file, `unique triangles: ${collisionGeometry.numUniqueTriangles}\n`);
    fs.writeSync(file, `single cell triangles: ${collisionGeometry.numSingleCellTriangles}\n`);
    fs.writeSync(file, `multi cell triangles: ${collisionGeometry.numMultiCellTriangles}\n`);
    fs.writeSync(file, "\n\n\n");

    for (const cell of collisionGeometry.cells) {
        fs.writeSync(file, `cell ${cell.posX},${cell.posZ}\n`);
        fs.writeSync(file, `    min: [x: ${cell.boundingBox.min[0]}, y: ${cell.boundingBox.min[1]}, z: ${cell.boundingBox.min[2]}] max [x: ${cell.boundingBox.max[0]}, y: ${cell.boundingBox.max[1]} z: ${cell.boundingBox.max[2]}]\n`);
        for (const boundingVolume of cell.boundingVolumes) {
            fs.writeSync(file, `    ${boundingVolume.node.name}  [w: ${boundingVolume.walls.length} f: ${boundingVolume.floors.length} c: ${boundingVolume.ceilings.length}] min: [x: ${boundingVolume.volume.min[0]}, z: ${boundingVolume.volume.min[2]}] max [x: ${boundingVolume.volume.max[0]}, z: ${boundingVolume.volume.max[2]}]\n`);
        }
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


function _createTriangleWireframesInPrim(name, triangles, color) {
    const mesh = new Mesh(name);
    const prim = new Primitive(Primitive.ElementType.Lines);
    prim.hasPositions = true;
    prim.hasVertexColors = true;
    prim.material = 0;
    mesh.primitives.push(prim);

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

    return mesh;
}

function _createBoundingMesh(name, boundingBox, boundingColor) {
    const mesh = new Mesh(name);
    const prim = new Primitive(Primitive.ElementType.Lines);
    prim.hasPositions = true;
    prim.hasVertexColors = true;
    prim.material = 0;
    mesh.primitives.push(prim);

    const t000 = prim.vertices.length;
    const p000 = [boundingBox.min[0], boundingBox.min[1], boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p000);
    prim.bounding.encapsulatePoint(p000);

    const t100 = prim.vertices.length;
    const p100 = [boundingBox.max[0], boundingBox.min[1], boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p100);
    prim.bounding.encapsulatePoint(p100);

    const t101 = prim.vertices.length;
    const p101 = [boundingBox.max[0], boundingBox.min[1], boundingBox.max[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p101);
    prim.bounding.encapsulatePoint(p101);

    const t001 = prim.vertices.length;
    const p001 = [boundingBox.min[0], boundingBox.min[1], boundingBox.max[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p001);
    prim.bounding.encapsulatePoint(p001);

    const t010 = prim.vertices.length;
    const p010 = [boundingBox.min[0], boundingBox.max[1], boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p010);
    prim.bounding.encapsulatePoint(p010);

    const t110 = prim.vertices.length;
    const p110 = [boundingBox.max[0], boundingBox.max[1], boundingBox.min[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p110);
    prim.bounding.encapsulatePoint(p110);

    const t111 = prim.vertices.length;
    const p111 = [boundingBox.max[0], boundingBox.max[1], boundingBox.max[2], 0, 0, 0, ...boundingColor];
    prim.vertices.push(p111);
    prim.bounding.encapsulatePoint(p111);

    const t011 = prim.vertices.length;
    const p011 = [boundingBox.min[0], boundingBox.max[1], boundingBox.max[2], 0, 0, 0, ...boundingColor];
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

    return mesh;
}

function createBoundingNode(scene, parentNode, gltfLoader, name, boundingBox, color, layerMask) {
    const node = LevelParser.createAndAddNode(scene, parentNode);
    node.name = name;
    node.collider = Node.NoCollider;
    node.layerMask = layerMask;

    const mesh = _createBoundingMesh(name, boundingBox, color);

    // bundle the mesh in the scene
    scene.meshBundle.push(gltfLoader.meshes.length)

    // assign the mesh and reserve a mesh instance for it
    node.mesh = gltfLoader.meshes.length
    scene.meshInstanceCount += 1;
    gltfLoader.meshes.push(mesh);
}

function createTriangleNode(scene, parentNode, gltfLoader, name, triangles, color, layerMask) {
    if (triangles.length === 0) {
        return;
    }

    const node = LevelParser.createAndAddNode(scene, parentNode);
    node.name = name;
    node.collider = Node.NoCollider;
    node.layerMask = layerMask;

    const mesh = _createTriangleWireframesInPrim(name, triangles, color);

    // bundle the mesh in the scene
    scene.meshBundle.push(gltfLoader.meshes.length)

    // assign the mesh and reserve a mesh instance for it
    node.mesh = gltfLoader.meshes.length
    scene.meshInstanceCount += 1;
    gltfLoader.meshes.push(mesh);

}

async function writeCollisionGeometryDebugData(environment, collisionGeometry, writeInterface, SceneWriter, filePath){
    const file = fs.openSync(filePath, "w");

    // allocate cell offset buffer and re-reserve space in file
    const cellOffsetBuffer = Buffer.alloc((collisionGeometry.cells.length + 1) * 4)
    fs.writeSync(file, cellOffsetBuffer);
    let offsetBufferIndex = 0;
    offsetBufferIndex = writeInterface.writeUInt32(cellOffsetBuffer, collisionGeometry.cells.length, offsetBufferIndex);

    // create a scene for each cell that will be written into the buffer
    for (const cell of collisionGeometry.cells) {
        const gltfLoader = new GLTFLoader();

        // setup the scene for this cell
        const scene = new Scene();
        scene.name = `cell_${cell.posX}_${cell.posZ}_collision_debug`;
        scene.materialBundle = new MaterialBundle(gltfLoader);

        const wireMaterial = new Material();
        wireMaterial.shadingMode = Material.ShadingMode.Line;
        gltfLoader.materials.push(wireMaterial);
        scene.materialBundle.materials.push(0);

        const rootNode = LevelParser.createAndAddNode(scene, null);
        rootNode.name = "root";
        rootNode.collider = Node.NoCollider;

        createBoundingNode(scene, rootNode, gltfLoader, `cell_${cell.posX}_${cell.posZ}`, cell.boundingBox, [1.0, 1.0, 1.0, 1.0], LayerMask.CellBoundings);

        for (const boundingVolume of cell.boundingVolumes) {
            createBoundingNode(scene, rootNode, gltfLoader, `volume`, boundingVolume.volume, [0.94, 0.92, 0.15, 1.0], LayerMask.BoundingVolumes);
            createTriangleNode(scene, rootNode, gltfLoader, "floors", boundingVolume.floors, [0.08, 0.18, 0.78, 1.0], LayerMask.FloorTriangles);
            createTriangleNode(scene, rootNode, gltfLoader, "walls", boundingVolume.walls, [0.06, 0.78, 0.03, 1.0], LayerMask.WallTriangles);
            createTriangleNode(scene, rootNode, gltfLoader, "ceilings", boundingVolume.walls, [0.85, 0.24, 0.10, 1.0], LayerMask.CeilingTriangles);
        }

        LevelParser.assignNodeChildPointers(scene);

        const stats = fs.statSync(filePath);
        offsetBufferIndex = writeInterface.writeUInt32(cellOffsetBuffer, stats.size, offsetBufferIndex);
        await SceneWriter.writeToFile(environment, scene, gltfLoader, file)
    }

    // write the actual cell data offsets into the file
    fs.writeSync(file, cellOffsetBuffer, 0, cellOffsetBuffer.byteLength, 0);
    fs.closeSync(file);
}

/** This needs to be kept in sync with fw64CollisionSceneManagerLayers in components/fw64_collision_scene_manager.h */
const CollisionDebugLayers = {
    DebugSceneWireTriangleLayer: 1,
    DebugSceneWireGridLayer: 2
};

module.exports = {
    writeTextFile: writeTextFile,
    writeCollisionGeometryDebugData: writeCollisionGeometryDebugData
}