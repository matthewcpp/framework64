#pragma once

#include "framework64/collider.h"

#include <vector>

namespace framework64{

struct MeshCollider : public fw64MeshCollider {
    std::vector<float> point_data;
    std::vector<uint16_t> element_data;
};

}