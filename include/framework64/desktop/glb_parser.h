#pragma once

#include "framework64/desktop/mesh.h"
#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/texture.h"

#include <gl/glew.h>
#include <nlohmann/json.hpp>

#include <fstream>

namespace framework64 {

class GlbParser {
public:
    GlbParser(ShaderCache& sc) : shader_cache(sc) {};
public:
    struct Options {
        bool bakeTransform = false;
    };
public:
    fw64Mesh* parseStaticMesh(std::string const & path);
    fw64Mesh* parseStaticMesh(std::string const & path, Options options);

private:
    bool parseHeader();
    bool parseJsonChunk();
    bool parseBinaryChunk();

    void parseMeshPrimitives(fw64Mesh* mesh);
    void parseMaterial(Material& material, size_t material_index);

    template <typename T>
    std::vector<T> readBufferViewData(size_t bufferViewIndex);

    template <typename T>
    std::vector<T> readPrimitiveAttributeBuffer(nlohmann::json const & primitive_node, const char* key);

    Box getBoxFromAccessor(size_t accessor_index) const;
    void seekInBinaryChunk(size_t pos);

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
    nlohmann::json json_doc;
    Options parse_options;
    ShaderCache& shader_cache;
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
}

