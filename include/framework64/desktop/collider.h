#pragma once

#include "framework64/collider.h"

#include <vector>

namespace framework64{

struct CollisionMesh : public fw64CollisionMesh {
    std::vector<float> point_data;
    std::vector<uint16_t> element_data;
};

}