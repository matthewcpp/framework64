/** Contains indices into data for one joint's inputs / outputs for a particular animation
  * E.G. The Right leg bone for the run animation
  * The indices in this class store _byte offsets_ into the animation data array
*/
class Track {
    translationInput = null;
    translation = null;

    rotationInput = null;
    rotation = null;

    scaleInput = null;
    scale = null;

    // this is the size of the c++ data structure
    static SizeOf = 12;
}

class Animation {
    name;
    time = 0.0;
    input = null;
    tracks = []; // Each track in this array corresponds to a join define din the gltf file.

    constructor(name) {
        this.name = name;
    }
}

module.exports = {
    Track: Track,
    Animation: Animation
}