#include "framework64/desktop/glb_parser.h"

#include "framework64/desktop/mesh_data.h"
#include "framework64/matrix.h"

#include <cassert>
#include <limits>
#include <iostream>

namespace framework64 {

constexpr uint32_t HeaderMagicNumber = 0x46546C67;
constexpr uint32_t JsonChunkMagicNumber = 0x4E4F534A;
constexpr uint32_t BinaryChunkMagicNumber = 0x004E4942;

enum GltfComponentType {
    UnsignedShort = 5123,
    UnsignedInt = 5125,
    Float = 5126
};

// note this function assumes scene 0 is the "active scene"
fw64Mesh* GlbParser::parseStaticMesh(std::string const & path) {
    if (!openFile(path))
        return nullptr;

    return createStaticMesh(json_doc["meshes"][0]);
}

std::vector<fw64Mesh*> GlbParser::parseStaticMeshes(std::string const & path) {
    std::vector<fw64Mesh*> meshes;

    if (!openFile(path))
        return meshes;

    for (auto const & mesh : json_doc["meshes"] ) {
        meshes.push_back(createStaticMesh(mesh));
    }

    return meshes;
}

bool GlbParser::openFile(std::string const& path) {
    loaded_textures.clear();

    glb_file.open(path, std::ios::binary);

    if (!glb_file)
        return false;

    if (!parseHeader() || !parseJsonChunk() || !parseBinaryChunk() )
        return false;

    if (json_doc.contains("textures")) {
        loaded_textures.resize(json_doc["textures"].size(), nullptr);
    }

    return true;
}

bool GlbParser::parseHeader() {
    glb_file.read(reinterpret_cast<char * >(&header), sizeof(Header));

    return header.magic == HeaderMagicNumber && header.version == 2;
}

bool GlbParser::parseJsonChunk() {
    glb_file.read(reinterpret_cast<char * >(&json_chunk_info), sizeof(ChunkInfo));

    if (json_chunk_info.type != JsonChunkMagicNumber)
        return false;

    std::vector<char> json_data(json_chunk_info.length);
    glb_file.read(json_data.data(), json_chunk_info.length);

    json_doc = nlohmann::json ::parse(json_data.begin(), json_data.end());

    return true;
}

bool GlbParser::parseBinaryChunk() {
    glb_file.read(reinterpret_cast<char * >(&binary_chunk_info), sizeof(ChunkInfo));

    return binary_chunk_info.type == BinaryChunkMagicNumber;
}

Box GlbParser::getBoxFromAccessor(size_t accessor_index) const {
    auto const & accessor_node = json_doc["accessors"][accessor_index];
    auto const & min_node = accessor_node["min"];
    auto const & max_node = accessor_node["max"];

    Box box;
    box.min.x = min_node[0].get<float>();
    box.min.y = min_node[1].get<float>();
    box.min.z = min_node[2].get<float>();

    box.max.x = max_node[0].get<float>();
    box.max.y = max_node[1].get<float>();
    box.max.z = max_node[2].get<float>();

    return box;
}

static fw64Primitive::Mode getPrimitiveMode(nlohmann::json const & primitive_node) {
    if (!primitive_node.contains("mode"))
        return fw64Primitive::Mode::Triangles;
    
    int mode = primitive_node["mode"].get<int>();

    switch (mode) {
        case 1:
            return fw64Primitive::Mode::Lines;
        case 4:
            return fw64Primitive::Mode::Triangles;
        default:
            return fw64Primitive::Mode::Unknown;
    }
}

// note this assumes there is only one mesh at the top level
// it will need to be modified in the future
static std::array<float, 16> extractMatrixFromNode(nlohmann::json const & node) {
    std::array<float, 16> transform;
    matrix_set_identity(transform.data());

    Vec3 translation = {0.0f, 0.0f, 0.0f};
    Quat rotation;
    quat_ident(&rotation);
    Vec3 scale = {1.0f, 1.0f, 1.0f};

    bool set_trs = true;

    if (node.contains("translation")) {
        auto const & translation_node = node["translation"];
        translation.x = translation_node[0].get<float>();
        translation.y = translation_node[1].get<float>();
        translation.z = translation_node[2].get<float>();
    }

    if (node.contains("rotation")) {
        auto const & rotation_node = node["rotation"];
        rotation.x = rotation_node[0].get<float>();
        rotation.y = rotation_node[1].get<float>();
        rotation.z = rotation_node[2].get<float>();
        rotation.w = rotation_node[3].get<float>();
    }

    if (node.contains("scale")) {
        auto const & scale_node = node["scale"];
        scale.x = scale_node[0].get<float>();
        scale.y = scale_node[1].get<float>();
        scale.z = scale_node[2].get<float>();
    }

    if (node.contains("matrix")) {
        auto const &matrix_node = node["matrix"];

        for (size_t i = 0; i < 16; i++) {
            transform[i] = matrix_node[i].get<float>();
        }

        set_trs = false;
    }

    if (set_trs) {
        matrix_from_trs(transform.data(), &translation, &rotation, &scale);
    }

    return transform;
}

fw64Mesh* GlbParser::createStaticMesh(nlohmann::json const & mesh_node) {
    auto mesh = new fw64Mesh();

    for (auto const & primitive_node : mesh_node["primitives"]) {
        auto const primitive_mode = getPrimitiveMode(primitive_node);

        if (primitive_mode == fw64Primitive::Mode::Unknown)
            continue;

        auto & primitive = mesh->primitives.emplace_back();

        primitive.mode = primitive_mode;

        if (primitive_node.contains("material"))
            parseMaterial(primitive.material, primitive_node["material"].get<size_t>());

        auto const & attributes_node = primitive_node["attributes"];
        MeshData mesh_data;

        mesh_data.positions = readPrimitiveAttributeBuffer<float>(attributes_node, "POSITION");
        mesh_data.normals = readPrimitiveAttributeBuffer<float>(attributes_node, "NORMAL");
        mesh_data.tex_coords = readPrimitiveAttributeBuffer<float>(attributes_node, "TEXCOORD_0");
        mesh_data.colors = parseVertexColors(primitive_node);

        if (!mesh_data.positions.empty()) {
            auto accessor = attributes_node["POSITION"].get<size_t>();
            primitive.bounding_box = getBoxFromAccessor(accessor);
            box_encapsulate_box(&mesh->bounding_box, &primitive.bounding_box);
        }

        auto const & element_accessor_node = json_doc["accessors"][primitive_node["indices"].get<size_t>()];
        auto componentType = element_accessor_node["componentType"].get<uint32_t>();
        auto bufferViewIndex = element_accessor_node["bufferView"].get<size_t>();

        if (componentType == GltfComponentType::UnsignedShort) {
            mesh_data.indices_array_uint16 = readBufferViewData<uint16_t>(bufferViewIndex);
        }
        else if (componentType == GltfComponentType::UnsignedInt) {
            mesh_data.indices_array_uint32 = readBufferViewData<uint32_t >(bufferViewIndex);
        }
        else {
            return nullptr; // error?
        }

        GLMeshInfo gl_info = mesh_data.createMesh();
        gl_info.setPrimitiveValues(primitive);
        shader_cache.setShaderProgram(primitive);
    }

    std::cout << mesh->bounding_box.min.x << ", " << mesh->bounding_box.min.y << ", " << mesh->bounding_box.min.z << ", "
              << mesh->bounding_box.max.x << ", " << mesh->bounding_box.max.y << ", " << mesh->bounding_box.max.z << std::endl;

    return mesh;
}

void GlbParser::parseMaterial(fw64Material& material, size_t material_index) {
    auto const & material_node = json_doc["materials"][material_index];
    auto const & pbr = material_node["pbrMetallicRoughness"];

    if (pbr.contains("baseColorFactor")) {
        auto const & base_color_factor = pbr["baseColorFactor"];

        for (size_t i = 0; i < 4; i++) {
            material.color[i] = base_color_factor[i].get<float>();
        }
    }

    if (pbr.contains("baseColorTexture")) {
        auto texture_index = pbr["baseColorTexture"]["index"].get<size_t>();
        material.texture = getTexture(texture_index);
    }
}

fw64Texture* GlbParser::getTexture(size_t texture_index) {
    if (loaded_textures[texture_index])
        return loaded_textures[texture_index];

    auto* texture = parseTexture(texture_index);
    loaded_textures[texture_index] = texture;

    return texture;
}

//TODO: read sampler parameters and apply to texture object
fw64Texture* GlbParser::parseTexture(size_t texture_index) {
    assert(loaded_textures[texture_index] == nullptr);

    auto source_index = json_doc["textures"][texture_index]["source"].get<size_t>();
    auto image_node = json_doc["images"][source_index];

    std::string image_name = image_node["name"].get<std::string>();

    auto buffer_view_index = image_node["bufferView"].get<size_t>();
    auto image_data = readBufferViewData<uint8_t>(buffer_view_index);

    auto* image = fw64Image::loadImageBuffer(reinterpret_cast<void *>(image_data.data()), image_data.size());
    auto* texture =  new fw64Texture(image);

    // Note: default wrap mode for GLTF is repeat
    texture->wrap_t = FW64_TEXTURE_WRAP_REPEAT;
    texture->wrap_s = FW64_TEXTURE_WRAP_REPEAT;

    return texture;
}

std::vector<float> GlbParser::parseVertexColors(nlohmann::json const & primitive_node) {
    std::vector<float> vertex_colors;

    auto const & attributes_node = primitive_node["attributes"];

    if (!attributes_node.contains("COLOR_0"))
        return vertex_colors;

    auto const & accessor_node = json_doc["accessors"][attributes_node["COLOR_0"].get<size_t>()];
    auto componentType = accessor_node["componentType"].get<uint32_t>();
    auto type = accessor_node["type"].get<std::string>();


    if (componentType == GltfComponentType::Float) {
        vertex_colors = readPrimitiveAttributeBuffer<float>(attributes_node, "COLOR_0");
    }
    else if (componentType == GltfComponentType::UnsignedShort){ // in this case values should be normalized
        auto vertex_colors_uint16 = readPrimitiveAttributeBuffer<uint16_t>(attributes_node, "COLOR_0");
        vertex_colors.resize(vertex_colors_uint16.size());
        std::transform(vertex_colors_uint16.begin(), vertex_colors_uint16.end(), vertex_colors.begin(), [](uint16_t u16) {
           return static_cast<float>(u16) / 65535.0f;
        });
    }

    if (!vertex_colors.empty() && type == "VEC3") {
        vertex_colors = vec3ToVec4Array<float>(vertex_colors, 1.0f);
    }

    return vertex_colors;
}

void GlbParser::seekInBinaryChunk(size_t pos) {
    size_t binary_start = sizeof(Header) + sizeof(json_chunk_info) + json_chunk_info.length + sizeof(binary_chunk_info);
    glb_file.seekg(binary_start + pos);
}

}
