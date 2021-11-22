#pragma once

#include "framework64/desktop/mesh_data.h"

#include <vector>

namespace framework64 {
    std::vector<MeshData> splitByJointIndex(MeshData const & mesh_data);
}
