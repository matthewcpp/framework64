const fs = require("fs");
const path = require("path");

class Parser {
    gltf = null;
    buffer = null;
    fileDir = null;

    loadGltf(filePath) {
        this.gltf = JSON.parse(fs.readFileSync(filePath, {encoding: "utf8"}));
        console.log(`Loaded GLTF: ${filePath}`);
        this.fileDir = path.dirname(filePath);
        const bufferPath = path.join(this.fileDir, this.gltf.buffers[0].uri);
        this.buffer = fs.readFileSync(bufferPath);
        console.log(`Loaded Buffer: ${bufferPath}`);
    }

    dumpMesh(file, meshIndex) {
        const mesh = this.gltf.meshes[meshIndex];

        fs.writeSync(file, `Mesh: ${mesh.name}\n`);

        for (let p = 0; p < mesh.primitives.length; p++) {
            const primitive = mesh.primitives[p];
            const attributes = primitive.attributes;
            fs.writeSync(file, `\tPrimitive: ${p}\n`);
            const vertexAccessor = this.gltf.accessors[attributes.POSITION];
            const vertexBufferView = this.gltf.bufferViews[vertexAccessor.bufferView];

            const jointAccessor = this.gltf.accessors[attributes.JOINTS_0];
            const jointBufferView = this.gltf.bufferViews[jointAccessor.bufferView];

            const weightAccessor = this.gltf.accessors[attributes.WEIGHTS_0];
            const weightBufferView = this.gltf.bufferViews[weightAccessor.bufferView];

            let vertexIndex = vertexBufferView.byteOffset;
            let jointIndex = jointBufferView.byteOffset;
            let weightIndex = weightBufferView.byteOffset;

            for (let i = 0; i < vertexAccessor.count; i++) {
                const x = this.buffer.readFloatLE(vertexIndex);
                const y = this.buffer.readFloatLE(vertexIndex + 4);
                const z = this.buffer.readFloatLE(vertexIndex + 8);

                const j0 = this.buffer.readUInt8(jointIndex);
                const j1 = this.buffer.readUInt8(jointIndex + 1);
                const j2 = this.buffer.readUInt8(jointIndex + 2);
                const j3 = this.buffer.readUInt8(jointIndex + 3);

                const w0 = this.buffer.readFloatLE(weightIndex);
                const w1 = this.buffer.readFloatLE(weightIndex + 4);
                const w2 = this.buffer.readFloatLE(weightIndex + 8);
                const w3 = this.buffer.readFloatLE(weightIndex + 12);

                vertexIndex += 12;
                jointIndex += 4;
                weightIndex += 16;

                fs.writeSync(file, `\t\t${x}, ${y}, ${z} ---> ${j0}: ${w0} ${j1}: ${w1}, ${j2}: ${w2}, ${j3}: ${w3}\n`);
            }
        }

        fs.writeSync(file, `\n\n\n`);
    }

    parseSkin(skinIndex) {
        const skin = this.gltf.skins[skinIndex];
        console.log(`Parse skin: ${skin.name}`);

        for (let i = 0; i < skin.joints.length; i++) {
            const node = this.gltf.nodes[skin.joints[i]];
            console.log(`Joint ${i}: ${node.name}`);
        }
    }

    dumpJoint(file, index, indent) {
        const node = this.gltf.nodes[index];

        for (let i = 0; i < indent; i++) {
            fs.writeSync(file, '\t');
        }

        fs.writeSync(file, `${node.name} (${index})` + '\n');

        if (node.children) {
            for (const child of node.children) {
                this.dumpJoint(file, child, indent + 1);
            }
        }
    }

    dumpJointHierarchy(file) {
        const skin = this.gltf.skins[0];

        const root = skin.hasOwnProperty("skeleton") ? skin.skeleton : skin.joints[0];
        this.dumpJoint(file, root, 0);

        fs.writeSync(file, `\n\n\n`);
    }

    dumpBindMatrices(file) {
        const skin = this.gltf.skins[0];

        const accessor = this.gltf.accessors[skin.inverseBindMatrices];
        const bufferView = this.gltf.bufferViews[accessor.bufferView];

        let index = bufferView.byteOffset;

        fs.writeSync(file, `Inv bind matrices: \n`);

        for (let i = 0; i < skin.joints.length; i++) {
            const node = this.gltf.nodes[skin.joints[i]];
            fs.writeSync(file, `Joint ${i}: ${node.name}: `);

            for (let m = 0; m < 16; m++) {
                if (m > 0)
                    fs.writeSync(file, ", ");
                    fs.writeSync(file, this.buffer.readFloatLE(index).toString());
                    index += 4;
            }

            fs.writeSync(file, "\n");
        }


        fs.writeSync(file, `\n\n\n`);
    }
}

if (require.main === module) {
    const parser = new Parser();
    parser.loadGltf(process.argv[2])

    const dumpPath = path.join(parser.fileDir, "mesh.dump");
    const file = fs.openSync(dumpPath, 'w');
    parser.dumpJointHierarchy(file);
    parser.dumpBindMatrices(file);
    parser.dumpMesh(file, 0);


    fs.closeSync(file);
}