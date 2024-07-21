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
    index = writeInterface.writeUInt32(buffer, animationData.totalTrackCount * 6, index);
    index = writeInterface.writeUInt32(buffer, animationData.keyframeDataSize, index);

    fs.writeSync(file, buffer);
}

function _writeAnimations(file, animationData, writeInterface) {
    const animationBuffer = Buffer.alloc(16);

    for (let i = 0; i < animationData.animations.length; i++) {
        const animation = animationData.animations[i];
        const inputBuffer = animationData.keyframeBuffers[animation.input];

        writeInterface.writeFloat(animationBuffer, animation.time, 0);
        writeInterface.writeUInt32(animationBuffer, inputBuffer.size, 4);
        writeInterface.writeUInt32(animationBuffer, inputBuffer.index / 4, 8);
        writeInterface.writeUInt32(animationBuffer, i * animationData.joints.length, 12);
        fs.writeSync(file, animationBuffer);
    }
}

function _writeTrackData(file, animationData, writeInterface) {
    const trackBuffer = Buffer.alloc(6);
    // Note: the indices for each track specify the byteOffset into the keyframe data array.
    // We are going to write the index into that array as if it was of type float[] (i.e divide the byte index by 4)
    // This is will make accessing the data easier in the animation controller
    for (const animation of animationData.animations) {
        for (const track of animation.tracks) {
            let translationIndex = 0;
            let rotationIndex = 0;
            let scaleIndex = 0;

            // node parse validates this will work when animation data is read
            if (animationData.keyframeBuffers[track.translation]) {
                translationIndex = animationData.keyframeBuffers[track.translation].index / 4;
                rotationIndex = animationData.keyframeBuffers[track.rotation].index / 4;
                scaleIndex = animationData.keyframeBuffers[track.scale].index / 4;
            }
            else {
                translationIndex = InvalidKeyframeBufferIndex;
                rotationIndex = InvalidKeyframeBufferIndex;
                scaleIndex = InvalidKeyframeBufferIndex;
            }

            writeInterface.writeUInt16(trackBuffer, translationIndex, 0);
            writeInterface.writeUInt16(trackBuffer, rotationIndex, 2);
            writeInterface.writeUInt16(trackBuffer, scaleIndex, 4);

            fs.writeSync(file, trackBuffer);
        }
    }
}

function _writeKeyframeData(file, animationData, writeInterface) {
    for (const keyframeDataSlice of animationData.keyframeDataSlices) {
        fs.writeSync(file, writeInterface.floatBufferToNative(keyframeDataSlice));
    }
}

function writeHeaderFile(animationData, meshName, destPath) {
    const safeSkinnedMeshString = meshName + "_animation";

    const animations = animationData.animations;

    const file = fs.openSync(destPath, 'w');
    fs.writeSync(file, "#pragma once\n\n");
    fs.writeSync(file, `#define ${safeSkinnedMeshString}__count__ ${animations.length}\n\n`);

    for (let i = 0; i < animations.length; i++) {
        const animation = animations[i];
        const safeAnimationName = Util.safeDefineName(animation.name);
        fs.writeSync(file, `#define ${safeSkinnedMeshString}_${safeAnimationName} ${i}\n`);
        fs.writeSync(file, `#define ${safeSkinnedMeshString}_${safeAnimationName}_name "${animation.name}"\n`);
    }

    fs.closeSync(file);
}

module.exports = {
    write: write,
    writeToFile: writeToFile,
    writeHeaderFile: writeHeaderFile
}