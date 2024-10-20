const Animation = require("./Animation")
const Util = require("../Util");

const crypto = require("crypto");

class KeyframeBuffer {
    size;
    index;

    constructor(size, index) {
        this.size = size;
        this.index = index;
    }
}

class Joint {
    name; // used only for debugging purposes.  this is extracted from the corresponding gltf node
    id; // the MAPPED index of this node
    originalIndex; // the original index in the skin's 'joints' array
    children = []; // MAPPED indices of all child joints
    inverseBindMatrix = null; // extracted from the skin's inverseBindMatrices buffer
    parent = null; // the MAPPED index of this joint's parent

    constructor(name, id, originalIndex) {
        this.name = name;
        this.id = id;
        this.originalIndex = originalIndex;
    }
}

class AnimationData {
    animations = [];
    jointIdMap = new Map();  // maps the joint node id to the joint array index
    joints = [];

    keyframeBuffers = [];
    keyframeDataSlices = [];
    keyframeDataSize = 0;

    get totalTrackCount() {
        let count = 0;

        for (const animation of this.animations) {
            count += animation.tracks.length;
        }

        return count;
    }
}

class BufferInfo {
    bufferViewIndexToChunk = new Map();
    bufferHashToChunk = new Map();
}

class Parser {
    data = null;
    gltf = null;
    /** NodeJS Buffer holding all the content of the gltf .bin file */
    buffer = null;
    options = null;

    animationFilter = null;
    buffers = new BufferInfo();

    // maps an index into the gltf node array to a Joint object
    nodeIndexToJointMap = new Map();

    /// TODO: this should be able to support gltf files /w multiple buffers and even glb files
    parse(gltf, buffer, options) {
        this.gltf = gltf;
        this.buffer = buffer;
        this.options = options;

        this.data = new AnimationData();
        this._setupAnimationFilter();

        this.parseSkin();
        this.parseAnimations();

        return this.data;
    }

    _setupAnimationFilter() {
        if (!this.options.animations)
            return;

        this.animationFilter = new Map();

        for (const animation of this.options.animations) {
            this.animationFilter.set(animation, false);
        }
    }

    parseSkin() {
        const sizeOfMatrix = 64;
        const skin = this.gltf.skins[0];
        const ibmAccessor = this.gltf.accessors[skin.inverseBindMatrices];
        const ibmBufferView = this.gltf.bufferViews[ibmAccessor.bufferView];

        //we will use a singular root node for the skin
        // todo: this is not necessary if the skin contains a 'skeleton' property
        const rootJoint = new Joint("Skeleton Root", 0, 255);
        rootJoint.inverseBindMatrix = Util.mat4IdentityBuffer();
        this.data.jointIdMap.set(this.data.joints.length, 255);
        this.data.joints.push(rootJoint);

        // create a object for each joint and add it to map
        // this will map the original index of the joint to the filterd joint array
        for (let i = 0; i < skin.joints.length; i++) {
            // todo: filter out unnecessary joints from the array
            const jointNode = this.gltf.nodes[skin.joints[i]];
            const joint = new Joint(jointNode.name, this.data.joints.length, i);

            const matrixIndex = ibmBufferView.byteOffset + (i * sizeOfMatrix);
            joint.inverseBindMatrix = this.buffer.slice(matrixIndex, matrixIndex + sizeOfMatrix);

            this.data.joints.push(joint);
            this.data.jointIdMap.set(joint.originalIndex, joint.id);
            this.nodeIndexToJointMap.set(skin.joints[i], joint);
        }

        // build the joint hierarchy by creating the child list for each node
        for (const joint of this.data.joints) {
            if (joint.originalIndex > skin.joints.length) // skip our fake skeleton root node
                continue;

            const jointNodeIndex = skin.joints[joint.originalIndex];
            const jointNode = this.gltf.nodes[jointNodeIndex];

            if (!jointNode.children)
                continue;

            for (const childNodeIndex of jointNode.children) {
                const childJoint = this.nodeIndexToJointMap.get(childNodeIndex);
                childJoint.parent = jointNode.id;
                joint.children.push(childJoint.id);
            }
        }

        // assign all top level nodes as children of the skeleton root
        //todo: this not necessary if the skeleton property specified in the joint
        for (let i = 1; i < this.data.joints.length; i++) {
            const joint = this.data.joints[i];

            if (joint.parent === null) {
                joint.parent = rootJoint.id;
                rootJoint.children.push(joint.id);
            }
        }
    }

    /**
     * This method acts as a cache for buffer chunks that will end up in the animation data array
     * given an accessor it will return an index (specified in bytes) of the start of that accessors
     * position in the animation data buffer.
     */
    getBufferChunk(accessorIndex) {
        const accessor = this.gltf.accessors[accessorIndex];

        const info = this.buffers;

        // this chunk is already in the buffer data
        if (info.bufferViewIndexToChunk.has(accessor.bufferView)) {
            return info.bufferViewIndexToChunk.get(accessor.bufferView);
        }

        // check if we have a chunk with the same hash
        const bufferView = this.gltf.bufferViews[accessor.bufferView];
        const bufferSlice = Buffer.allocUnsafe(bufferView.byteLength + 4); // note we will write the length into the start of this buffer
        this.buffer.copy(bufferSlice, 4, bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength);
        this.buffer.writeFloatLE(bufferSlice, accessor.count, 0);

        // TODO: is crypto necessary here?  can we simply cache the accessor index???
        const hash = crypto.createHash('md5').update(bufferSlice).digest('hex');

        if (info.bufferHashToChunk.has(hash)) {
            return info.bufferHashToChunk.get(hash);
        }

        // new chunk, add it to the list and then cache it
        const chunkIndex = this.data.keyframeBuffers.length;

        this.data.keyframeBuffers.push(new KeyframeBuffer(accessor.count, this.data.keyframeDataSize));
        this.data.keyframeDataSlices.push(bufferSlice);
        this.data.keyframeDataSize += bufferSlice.byteLength;

        info.bufferViewIndexToChunk.set(accessorIndex, chunkIndex);
        info.bufferHashToChunk.set(hash, chunkIndex);

        return chunkIndex;
    }

    _shouldParseAnimation(animation) {
        if (!this.animationFilter)
            return true;
        else
            return this.animationFilter.has(animation);
    }

    _createAnimationTracks(animation) {
        for (let i = 0; i < this.data.joints.length; i++) {
            animation.tracks.push(new Animation.Track());
        }
    }

    parseAnimations() {
        for (const gltfAnimation of this.gltf.animations) {
            if (!this._shouldParseAnimation(gltfAnimation.name))
                continue;

            const animation = new Animation.Animation(gltfAnimation.name);
            this._createAnimationTracks(animation);

            for (const channel of gltfAnimation.channels) {
                const sampler = gltfAnimation.samplers[channel.sampler];

                animation.time = Math.max(animation.time, this.gltf.accessors[sampler.input].max);

                // Note: Currently only supporting linear animation
                const inputChunk = this.getBufferChunk(sampler.input);
                const outputChunk = this.getBufferChunk(sampler.output);

                let joint = this.nodeIndexToJointMap.get(channel.target.node);
                if (!joint) {
                    joint = this.data.joints[0];
                    // if we get here it means that there is an animation channel setup for a node which is not listed as a
                    // joint in the skeleton.  This could mean that there is animated channels on the armature itself.
                    
                    //console.log(`node not found ${gltfAnimation.name} node: ${channel.target.node}`);
                    //continue;
                }

                const track = animation.tracks[joint.id];

                switch (channel.target.path) {
                    case "translation":
                        track.translationInput = inputChunk;
                        track.translation = outputChunk;
                    break;

                    case "rotation":
                        track.rotationInput = inputChunk;
                        track.rotation = outputChunk;
                    break;

                    case "scale":
                        track.scaleInput = inputChunk;
                        track.scale = outputChunk;
                        break;
                }
            }

            if (!this._validateAnimation(animation)) {
                throw new Error("Animation tracks not specified");
            }

            this.data.animations.push(animation);

            if (this.animationFilter) {
                this.animationFilter.set(animation.name, true);
            }
        }

        this._checkParsedAnimations();
    }

    _validateAnimation(animation) {
        for (let i = 1; i < animation.tracks.length; i++) {
            const track = animation.tracks[i];

            if (track.translation === null || track.rotation === null|| track.scale === null)
                return false;
        }

        return true;
    }

    _checkParsedAnimations() {
        if (!this.animationFilter)
            return;

        const unparsedAnimations = [];

        this.animationFilter.forEach((value, key) => {
            if (!value)
                unparsedAnimations.push(key);
        });

        if (unparsedAnimations.length > 0) {
            throw new Error("Unable to locate animations: " + unparsedAnimations.join(' '));
        }
    }
}

module.exports = Parser;