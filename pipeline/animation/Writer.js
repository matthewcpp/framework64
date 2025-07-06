const Animation = require("./Animation")
const Util = require("../Util");

const path = require("path");
const fs = require("fs");

const InvalidKeyframeBufferIndex = 65535;

function jointHierarchyArray(animationData) {
    let joints = [];
    let children = [];

    for (const joint of animationData.joints) {
        joints.push(children.length);
        joints.push(joint.children.length);
        children.push(...joint.children);
    }

    return joints.concat(children);
}

function write(animationData, destPath, writeInterface) {
    const file = fs.openSync(destPath, "w");
    writeToFile(animationData, writeInterface, file);
    fs.closeSync(file);
}

function writeToFile(animationData, writeInterface, file) {
    const jointHierarchy = jointHierarchyArray(animationData);

    _writeAnimationInfo(animationData, jointHierarchy, writeInterface, file)

    fs.writeSync(file, writeInterface.int16ArrayToBuffer(jointHierarchy));
    for (const joint of animationData.joints) {
        fs.writeSync(file, writeInterface.floatBufferToNative(joint.inverseBindMatrix));
    }

    _writeAnimations(file, animationData, writeInterface);
    _writeTrackData(file, animationData, writeInterface)
    _writeKeyframeData(file, animationData, writeInterface);
}

function _writeAnimationInfo(animationData, jointHierarchy, writeInterface, file) {
    const bufferSize = 20;
    const buffer = Buffer.alloc(bufferSize);

    let index = 0;
    index = writeInterface.writeUInt32(buffer, animationData.joints.length, index);
    index = writeInterface.writeUInt32(buffer, jointHierarchy.length * 2, index);
    index = writeInterface.writeUInt32(buffer, animationData.animations.length, index);
    index = writeInterface.writeUInt32(buffer, animationData.totalTrackCount * Animation.Track.SizeOf, index);
    index = writeInterface.writeUInt32(buffer, animationData.keyframeDataSize, index);

    fs.writeSync(file, buffer);
}

function _writeAnimations(file, animationData, writeInterface) {
    // The size of this buffer needs to align with fw64Animation in animation-data.h
    const animationBuffer = Buffer.alloc(8);

    for (let i = 0; i < animationData.animations.length; i++) {
        const animation = animationData.animations[i];

        writeInterface.writeFloat(animationBuffer, animation.time, 0);
        writeInterface.writeUInt32(animationBuffer, i * animationData.joints.length, 4);
        fs.writeSync(file, animationBuffer);
    }
}

function _writeTrackData(file, animationData, writeInterface) {
    const trackBuffer = Buffer.alloc(Animation.Track.SizeOf);
    // Note: the indices for each track specify the byteOffset into the keyframe data array.
    // We are going to write the index into that array as if it was of type float[] (i.e divide the byte index by 4)
    // This is will make accessing the data easier in the animation controller
    for (const animation of animationData.animations) {

        // temporary for setting up the c++ code
        // want to simulate reading from the individual buffers, but in reality they will all be the same
        const inputBuffer = animationData.keyframeBuffers[animation.input];

        for (const track of animation.tracks) {
            let translationInput = InvalidKeyframeBufferIndex;
            let translationIndex = InvalidKeyframeBufferIndex;
            let rotationInput = InvalidKeyframeBufferIndex;
            let rotationIndex = InvalidKeyframeBufferIndex;
            let scaleInput = InvalidKeyframeBufferIndex;
            let scaleIndex = InvalidKeyframeBufferIndex;

            // node parse validates this will work when animation data is read
            if (animationData.keyframeBuffers[track.translation]) {
                translationInput = animationData.keyframeBuffers[track.translationInput].index / 4;
                translationIndex = animationData.keyframeBuffers[track.translation].index / 4;

                rotationInput = animationData.keyframeBuffers[track.rotationInput].index / 4;
                rotationIndex = animationData.keyframeBuffers[track.rotation].index / 4;

                scaleInput = animationData.keyframeBuffers[track.scaleInput].index / 4;
                scaleIndex = animationData.keyframeBuffers[track.scale].index / 4;
            }

            writeInterface.writeUInt16(trackBuffer, translationInput, 0);
            writeInterface.writeUInt16(trackBuffer, translationIndex, 2);

            writeInterface.writeUInt16(trackBuffer, rotationInput, 4);
            writeInterface.writeUInt16(trackBuffer, rotationIndex, 6);

            writeInterface.writeUInt16(trackBuffer, scaleInput, 8);
            writeInterface.writeUInt16(trackBuffer, scaleIndex, 10);

            fs.writeSync(file, trackBuffer);
        }
    }
}

function _writeKeyframeData(file, animationData, writeInterface) {
    for (let i = 0; i < animationData.keyframeDataSlices.length; i++) {
        const keyframeDataSlice = animationData.keyframeDataSlices[i];
        const keyframeBuffer = animationData.keyframeBuffers[i];

        const nativeBuffer = writeInterface.floatBufferToNative(keyframeDataSlice);
        // We are going to prepend the the number of items in the buffer
        // This is used by the animation system to know the size of actual items (Vec3, Quat) in the buffer
        writeInterface.writeUInt32(nativeBuffer, keyframeBuffer.size, 0);

        fs.writeSync(file, nativeBuffer);
    }
}

function writeHeaderFile(animationData, meshName, destPath) {
    const file = fs.openSync(destPath, 'w');
    fs.writeSync(file, "#pragma once\n\n");

    let safeSkinnedMeshString = meshName + "_animation";
    const animations = animationData.animations;

    fs.writeSync(file, `#define ${safeSkinnedMeshString}__count__ ${animations.length}\n\n`);

    for (let i = 0; i < animations.length; i++) {
        const animation = animations[i];
        const safeAnimationName = Util.safeDefineName(animation.name);
        fs.writeSync(file, `#define ${safeSkinnedMeshString}_${safeAnimationName} ${i}\n`);
        fs.writeSync(file, `#define ${safeSkinnedMeshString}_${safeAnimationName}_name "${animation.name}"\n`);
    }

    fs.writeSync(file, "\n\n");

    safeSkinnedMeshString = meshName + "_joint";
    const joints = animationData.joints;
    fs.writeSync(file, `#define ${safeSkinnedMeshString}__count__ ${joints.length}\n\n`);

    for (let i = 0; i < joints.length; i++) {
        const joint = joints[i];
        const safeJointName = Util.safeDefineName(joint.name);
        fs.writeSync(file, `#define ${safeSkinnedMeshString}_${safeJointName} ${i}\n`);
        fs.writeSync(file, `#define ${safeSkinnedMeshString}_${safeJointName}_name "${joint.name}"\n`);
    }

    fs.closeSync(file);
}

module.exports = {
    write: write,
    writeToFile: writeToFile,
    writeHeaderFile: writeHeaderFile
}