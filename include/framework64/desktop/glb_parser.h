#pragma once

#include "framework64/scene.h"

#include "framework64/desktop/collider.h"
#include "framework64/desktop/json_map.h"
#include "framework64/desktop/mesh.h"
#include "framework64/desktop/mesh_data.h"
#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/texture.h"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <nlohmann/json.hpp>

#include <array>
#include <fstream>
#include <string>
#include <unordered_map>

#define GLTF_INVALID_INDEX (-1)

namespace framework64 {

class GlbParser {
public:
    GlbParser(ShaderCache& sc) : shader_cache(sc) {};

public:
    /** Extracts a single, static mesh from the GLB file. */
    fw64Mesh* parseStaticMesh(std::string const & path);
    fw64Scene* parseScene(std::string const & path, int rootNodeIndex, TypeMap const & type_map, LayerMap const & layer_map);

    std::vector<fw64Mesh*> parseStaticMeshes(std::string const & path);

private:
    bool parseHeader();
    bool parseJsonChunk();
    bool parseBinaryChunk();

    fw64Mesh* createStaticMesh(nlohmann::json const & node);

    /** Parses a scene Root node in order to get indices for scene and mesh collider container nodes. */
    void parseSceneNode(int rootNodeIndex);

    MeshData readPrimitiveMeshData(nlohmann::json const & primitive_node);

    void parseMaterial(fw64Material& material, size_t material_index);

    fw64Texture* getTexture(size_t texture_index);
    fw64Texture* parseTexture(size_t texture_index);
    fw64Image* getImage(size_t image_index);
    fw64Image* parseImage(size_t image_index);

    std::vector<float> parseVertexColors(nlohmann::json const & primitive_node);

    fw64CollisionMesh * getCollisionMesh(std::string const & name);
    int findCollisionMeshIndex(std::string const& name);
    framework64::CollisionMesh* parseCollisionMesh(nlohmann::json const & mesh_node);

    template <typename T>
    std::vector<T> readBufferViewData(size_t bufferViewIndex);

    template <typename T>
    std::vector<T> readPrimitiveAttributeBuffer(nlohmann::json const & primitive_node, const char* key);

    template <typename T>
    static std::vector<T> vec3ToVec4Array(std::vector<T> const & vec3_arr, T fill_val);

    [[nodiscard]] Box getBoxFromAccessor(size_t accessor_index) const;
    void seekInBinaryChunk(size_t pos);

    bool openFile(std::string const& path);

    void resetMaps();

private:
    struct Header {
        uint32_t magic;
        uint32_t version;
        uint32_t length;
    };

    struct ChunkInfo {
        uint32_t length;
        uint32_t type;
    };

    Header header;
    ChunkInfo json_chunk_info;
    ChunkInfo binary_chunk_info;

    std::ifstream glb_file;
    std::string file_path;
    nlohmann::json json_doc;
    ShaderCache& shader_cache;

    /* Map gltf resource indices to object pointers.  Useful for lookup when multiple nodes reference the same resources*/
    std::unordered_map<size_t, fw64Texture*> gltfToTexture;
    std::unordered_map<size_t, fw64Mesh*> gltfToMesh;
    std::unordered_map<size_t, fw64Image*> gltfToImage;
    /* --- */

    fw64Scene* scene = nullptr;
    int scene_node_index = GLTF_INVALID_INDEX;
    int collider_node_index = GLTF_INVALID_INDEX;
};

    template <typename T>
    std::vector<T> GlbParser::readBufferViewData(size_t bufferViewIndex) {
        auto const & buffer_view = json_doc["bufferViews"][bufferViewIndex];
        auto const byte_length = buffer_view["byteLength"].get<size_t>();
        auto const byte_offset = buffer_view["byteOffset"].get<size_t>();

        std::vector<T> buffer_data(byte_length / sizeof(T));

        seekInBinaryChunk(byte_offset);
        glb_file.read(reinterpret_cast<char*>(buffer_data.data()), byte_length);

        return buffer_data;
    }

    template <typename T>
    std::vector<T> GlbParser::readPrimitiveAttributeBuffer(nlohmann::json const & attributes_node, const char* key) {
        std::vector<T> attribute_data;

        if (attributes_node.contains(key)) {
            auto accessor = attributes_node[key].get<size_t>();
            auto const& accessor_node = json_doc["accessors"][accessor];
            attribute_data = readBufferViewData<T>(accessor_node["bufferView"].get<size_t>());
        }

        return attribute_data;
    }

    template <typename T>
    std::vector<T> GlbParser::vec3ToVec4Array(std::vector<T> const & vec3_arr, T fill_val) {
        size_t item_count = vec3_arr.size() / 3;
        std::vector<T> vec4_arr(vec3_arr.size() + item_count);

        for (size_t i = 0; i < item_count; i++) {
            size_t src_index = i * 3;
            size_t dest_index = i * 4;

            vec4_arr[dest_index] = vec3_arr[src_index];
            vec4_arr[dest_index + 1] = vec3_arr[src_index + 1];
            vec4_arr[dest_index + 2] = vec3_arr[src_index + 2];
            vec4_arr[dest_index + 3] = fill_val;
        }

        return vec4_arr;
    }
}

