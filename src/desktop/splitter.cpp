#include "framework64/desktop/splitter.h"

#include <cassert>
#include <iterator>
#include <unordered_map>

namespace framework64 {

static std::unordered_map<uint32_t, std::vector<uint16_t>> partitionFaces(MeshData const & source_mesh) {
    const auto& face_list = source_mesh.indices_array_uint16;
    std::unordered_map<uint32_t, std::vector<uint16_t>> partitions;

    for (size_t i = 0; i < face_list.size(); i += 3) {
        size_t vertex_joint_index = face_list[i] * 4; // joints are stored as vec4
        uint8_t joint_index = source_mesh.joint_indices[vertex_joint_index];

        auto& faces = partitions[joint_index];
        faces.push_back(face_list[i]);
        faces.push_back(face_list[i + 1]);
        faces.push_back(face_list[i + 2]);
    }

    return partitions;
}

void push_vertex(MeshData const & source, uint16_t source_index, MeshData & dest) {
    int position_index = source_index * 3;
    std::copy(source.positions.begin() + position_index, source.positions.begin() + (position_index + 3), std::back_inserter(dest.positions));

    int joint_index = source_index * 4;
    std::copy(source.joint_indices.begin() + joint_index, source.joint_indices.begin() + (joint_index + 4), std::back_inserter(dest.joint_indices));

    if (!source.normals.empty()) {
        int normal_index = source_index * 3;
        std::copy(source.normals.begin() + normal_index, source.normals.begin() + (normal_index + 3), std::back_inserter(dest.normals));
    }

    if (!source.tex_coords.empty()) {
        int tex_coord_index = source_index * 2;
        std::copy(source.tex_coords.begin() + tex_coord_index, source.tex_coords.begin() + (tex_coord_index + 2), std::back_inserter(dest.tex_coords));
    }

    if (!source.colors.empty()) {
        int vertex_color_index = source_index * 4;
        std::copy(source.colors.begin() + vertex_color_index, source.colors.begin() + (vertex_color_index + 4), std::back_inserter(dest.colors));
    }
}

void createSplitMesh(MeshData const & source, std::vector<uint16_t> const & face_list, MeshData & dest) {
    std::unordered_map<size_t, size_t> source_to_dest_vertex; // maps the original vertex index to the new index
    uint16_t vertex_count = 0;

    for (const auto source_vertex_index : face_list) {
        auto mapped_vertex = source_to_dest_vertex.find(source_vertex_index);

        // we have already added this vertex to the cache
        if (mapped_vertex != source_to_dest_vertex.end()) {
            dest.indices_array_uint16.push_back(mapped_vertex->second);
            continue;
        }

        // havent encountered this index yet, need to add it to the dest mesh data and create a mapping for it
        auto new_vertex_index = vertex_count++;
        push_vertex(source, source_vertex_index, dest);
        dest.indices_array_uint16.push_back(new_vertex_index);
        source_to_dest_vertex[source_vertex_index] = new_vertex_index;
    }
}


std::vector<MeshData> splitByJointIndex(MeshData const & mesh_data) {
    assert(!mesh_data.indices_array_uint16.empty() && mesh_data.indices_array_uint32.empty());

    auto partitions = partitionFaces(mesh_data);

    std::vector<MeshData> split_data;

    for (const auto & partition : partitions) {
        auto & split_mesh = split_data.emplace_back();
        createSplitMesh(mesh_data, partition.second, split_mesh);
    }

    return split_data;
}

}