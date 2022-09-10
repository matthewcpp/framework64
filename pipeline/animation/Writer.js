const Util = require("../Util");
const path = require("path");
const fs = require("fs");

const WriteInterface = require("../WriteInterface");

const InvalidKeyframeBufferIndex = 65535;

class AnimationInfo {
    jointCount = 0;
    jointHierarchySize = 0
    animationCount = 0;
    trackDataSize = 0;
    keyframeDataSize = 0;

    toBuffer(writeInterface) {
        const bufferSize = 20;
        const buffer = Buffer.alloc(bufferSize);

        let index = 0;
        index = writeInterface.writeUInt32(buffer, this.jointCount, index);
        index = writeInterface.writeUInt32(buffer, this.jointHierarchySize, index);
        index = writeInterface.writeUInt32(buffer, this.animationCount, index);
        index = writeInterface.writeUInt32(buffer, this.trackDataSize, index);
        index = writeInterface.writeUInt32(buffer, this.keyframeDataSize, index);

        return buffer;
    }
}

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

class Writer {
    writeBigEndian(name, animationData, assetDir, includeDir) {
        this._write(name, animationData, assetDir, includeDir, WriteInterface.bigEndian());
    }

    writeLittleEndian(name, animationData, assetDir, includeDir) {
        this._write(name, animationData, assetDir, includeDir, WriteInterface.littleEndian());
    }

    _write(name, animationData, assetDir, includeDir, writeInterface) {
        const safeName = Util.safeDefineName(name);
        this._writeHeader(safeName, animationData, includeDir);

        const dataPath = path.join(assetDir, safeName + ".animation");
        const file = fs.openSync(dataPath, "w");

        const jointHierarchy = jointHierarchyArray(animationData);

        const animationInfo = new AnimationInfo();
        animationInfo.jointCount = animationData.joints.length;
        animationInfo.jointHierarchySize = jointHierarchy.length * 2;
        animationInfo.animationCount = animationData.animations.length;
        animationInfo.trackDataSize = animationData.totalTrackCount * 6;
        animationInfo.keyframeDataSize = animationData.keyframeDataSize;
        fs.writeSync(file, animationInfo.toBuffer(writeInterface));

        fs.writeSync(file, writeInterface.int16ArrayToBuffer(jointHierarchy));
        for (const joint of animationData.joints) {
            fs.writeSync(file, writeInterface.floatBufferToNative(joint.inverseBindMatrix));
        }

        this._writeAnimations(file, animationData, writeInterface);
        this._writeTrackData(file, animationData, writeInterface)
        this._writeKeyframeData(file, animationData, writeInterface);


        fs.closeSync(file);
    }

    _writeAnimations(file, animationData, writeInterface) {
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

    _writeTrackData(file, animationData, writeInterface) {
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

    _writeKeyframeData(file, animationData, writeInterface) {
        for (const keyframeDataSlice of animationData.keyframeDataSlices) {
            fs.writeSync(file, writeInterface.floatBufferToNative(keyframeDataSlice));
        }
    }

    _writeHeader(name, animationData, includeDir) {
        const safeSkinnedMeshString = Util.safeDefineName(`${name}_animation`);
        const includeFilePath = path.join(includeDir, safeSkinnedMeshString + ".h");

        const animations = animationData.animations;

        const file = fs.openSync(includeFilePath, 'w');
        fs.writeSync(file, "#pragma once\n\n");
        fs.writeSync(file, `#define ${safeSkinnedMeshString}__count__ ${animations.length}\n\n`);

        for (let i = 0; i < animations.length; i++) {
            const animation = animations[i];
            const safeAnimationName = Util.safeDefineName(animation.name);
            fs.writeSync(file, `#define ${safeSkinnedMeshString}_${safeAnimationName} ${i}\n`);
        }

        fs.closeSync(file);
    }
}

module.exports = Writer;