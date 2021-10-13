#pragma once

#include "framework64/transform.h"
#include "framework64/desktop/mesh.h"
#include "framework64/scene.h"

struct fw64Scene{
    fw64Transform transform;
    std::vector<fw64Mesh*> meshes;
    std::vector<fw64SceneExtra> extras;
};