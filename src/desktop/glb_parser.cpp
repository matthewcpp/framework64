#include "framework64/desktop/glb_parser.h"

namespace framework64 {

constexpr uint32_t HeaderMagicNumber = 0x46546C67;
constexpr uint32_t JsonChunkMagicNumber = 0x4E4F534A;
constexpr uint32_t BinaryChunkMagicNumber = 0x004E4942;

fw64Mesh* GlbParser::parseStaticMesh(std::string const & path) {
    glb_file.open(path, std::ios::binary);

    if (!glb_file)
        return nullptr;

    if (!parseHeader() || !parseJsonChunk() || !parseBinaryChunk() )
        return nullptr;

    auto static_mesh = std::make_unique<fw64Mesh>();

    parseMeshPrimitives(static_mesh.get());

    return static_mesh.release();
}

fw64Mesh* GlbParser::parseStaticMesh(std::string const & path, Options options) {
    parse_options = options;
    return parseStaticMesh(path);
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

static fw64Mesh::Primitive::Mode getPrimitiveMode(nlohmann::json const & primitive_node) {
    if (!primitive_node.contains("mode"))
        return fw64Mesh::Primitive::Mode::Triangles;
    
    int mode = primitive_node["mode"].get<int>();

    switch (mode) {
        case 1:
            return fw64Mesh::Primitive::Mode::Lines;
        case 4:
            return fw64Mesh::Primitive::Mode::Triangles;
        default:
            return fw64Mesh::Primitive::Mode::Unknown;
    }
}

void GlbParser::parseMeshPrimitives(fw64Mesh* mesh) {
    auto const & mesh_node = json_doc["meshes"][0];

    for (auto const & primitive_node : mesh_node["primitives"]) {
        auto const primitive_mode = getPrimitiveMode(primitive_node);

        if (primitive_mode == fw64Mesh::Primitive::Mode::Unknown)
            continue;

        auto & primitive = mesh->primitives.emplace_back();

        primitive.mode = primitive_mode;
        parseMaterial(primitive.material, primitive_node["material"].get<size_t>());

        auto const & attributes_node = primitive_node["attributes"];

        std::vector<float> positions = readPrimitiveAttributeBuffer<float>(attributes_node, "POSITION");
        std::vector<float> normals = readPrimitiveAttributeBuffer<float>(attributes_node, "NORMAL");
        std::vector<float> tex_coords = readPrimitiveAttributeBuffer<float>(attributes_node, "TEXCOORD_0");

        GLuint array_buffer_size = (positions.size() + normals.size() + tex_coords.size()) * sizeof(float);
        glGenVertexArrays(1, &primitive.gl_vertex_array_object);
        glBindVertexArray(primitive.gl_vertex_array_object);

        glGenBuffers(1, &primitive.gl_array_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, primitive.gl_array_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, array_buffer_size, nullptr, GL_STATIC_DRAW);
        GLsizeiptr buffer_offset = 0;

        if (!positions.empty()) {
            auto accessor = attributes_node["POSITION"].get<size_t>();
            primitive.bounding_box = getBoxFromAccessor(accessor);
            box_encapsulate_box(&mesh->bounding_box, &primitive.bounding_box);
            primitive.attributes |= fw64Mesh::Primitive::Attributes::Positions;
            GLsizeiptr data_size = positions.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, positions.data());
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
            buffer_offset += data_size;
        }

        if (!normals.empty()) {
            GLsizeiptr data_size = normals.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, normals.data());
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
            buffer_offset += data_size;
            primitive.attributes |= fw64Mesh::Primitive::Attributes::Normals;
        }

        if (!tex_coords.empty()) {
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, tex_coords.size() * sizeof(float), tex_coords.data());
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
            primitive.attributes |= fw64Mesh::Primitive::Attributes::TexCoords;
        }

        auto const & element_accessor_node = json_doc["accessors"][primitive_node["indices"].get<size_t>()];
        std::vector<uint16_t> elements = readBufferViewData<uint16_t>(element_accessor_node["bufferView"].get<size_t>());

        glGenBuffers(1, &primitive.gl_array_buffer_object);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.gl_array_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(uint16_t), elements.data(), GL_STATIC_DRAW);
        primitive.element_count = elements.size();

        glBindVertexArray(0);

        primitive.material.shader = shader_cache.getShaderProgram(primitive);
    }
}

void GlbParser::parseMaterial(Material& material, size_t material_index) {
    auto const & material_node = json_doc["materials"][material_index];
    auto const & pbr = material_node["pbrMetallicRoughness"];
    auto const & base_color_factor = pbr["baseColorFactor"];

    for (size_t i = 0; i < 4; i++) {
        material.color[i] = base_color_factor[i].get<float>();
    }
}

void GlbParser::seekInBinaryChunk(size_t pos) {
    size_t binary_start = sizeof(Header) + sizeof(json_chunk_info) + json_chunk_info.length + sizeof(binary_chunk_info);
    glb_file.seekg(binary_start + pos);
}

}
