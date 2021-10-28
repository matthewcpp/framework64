#include "framework64/desktop/glb_parser.h"

#include "framework64/desktop/scene.h"
#include "framework64/matrix.h"

#include <cassert>
#include <exception>
#include <sstream>

namespace framework64 {

constexpr uint32_t HeaderMagicNumber = 0x46546C67;
constexpr uint32_t JsonChunkMagicNumber = 0x4E4F534A;
constexpr uint32_t BinaryChunkMagicNumber = 0x004E4942;

enum GltfComponentType {
    UnsignedShort = 5123,
    UnsignedInt = 5125,
    Float = 5126
};

static void extractTransformFromNode(nlohmann::json const & node, Vec3& position, Quat& rotation, Vec3& scale);

// note this function assumes scene 0 is the "active scene"
fw64Mesh* GlbParser::parseStaticMesh(std::string const & path) {
    if (!openFile(path))
        return nullptr;

    return createStaticMesh(json_doc["meshes"][0]);
}

void GlbParser::parseSceneNode() {
    int scene_index = json_doc["scene"].get<int>();
    auto const & scene_node = json_doc["scenes"][scene_index];
    auto const & node_arr = scene_node["nodes"];

    for (auto const & node_index : node_arr) {
        int index = node_index.get<int>();
        const auto & node = json_doc["nodes"][index];

        if (node.contains("name")) {
            std::string node_name = node["name"].get<std::string>();

            if (node_name == "Scene") {
                scene_node_index = index;
            }
            else if (node_name == "Colliders") {
                collider_node_index = index;
            }
        }
    }
}

fw64Scene* GlbParser::parseScene(std::string const & path, TypeMap const & type_map, LayerMap const & layer_map) {
    if (!openFile(path))
        return nullptr;

    scene = new fw64Scene();

    if (json_doc.contains("meshes")) {
        scene->meshes.resize(json_doc["meshes"].size());
    }

    parseSceneNode();
    assert(scene_node_index != -1);

    // this node corresponds to the top level node in the scene of which all scene nodes are children
    // not to be confused with a scene object in gltf
    auto const & scene_node = json_doc["nodes"][scene_node_index];
    if (!scene_node.contains("children")) // empty scene
        return scene;

    for (auto const & child_index : scene_node["children"]) {
        auto const & node = json_doc["nodes"][child_index.get<int>()];

        bool has_mesh = node.contains("mesh");
        bool has_extras = node.contains("extras");

        if (!has_mesh && !has_extras)
            continue;

        auto* n = scene->createNode();
        fw64_node_init(n);

        fw64Mesh* mesh = nullptr;
        fw64CollisionMesh * collision_mesh = nullptr;

        if (has_mesh) {
            int mesh_index = node["mesh"].get<int>();

            if (scene->meshes[mesh_index] == nullptr) {
                mesh = createStaticMesh(json_doc["meshes"][mesh_index]);
                scene->meshes[mesh_index].reset(mesh);
            }
            else {
                mesh = scene->meshes[mesh_index].get();
            }
        }

        if (node.contains("extras")) {
            auto& extras = node["extras"];

            if (extras.contains("type")) {
                auto type_name = extras["type"].get<std::string>();
                auto result = type_map.find(type_name);

                assert (result != type_map.end());
                n->type = result->second;

            }

            if (extras.contains("layers")) {
                auto layers_str = extras["layers"].get<std::string>();
                uint32_t layer_mask = 0U;

                std::istringstream ss(layers_str);
                for (std::string layer_name; ss >> layer_name; ) {
                    auto result = layer_map.find(layer_name);

                    assert (result != layer_map.end());
                    layer_mask |= result->second;
                }

                n->layer_mask = layer_mask;
            }

            if (extras.contains("collider")) {
                std::string collision_mesh_name = extras["collider"].get<std::string>();
                collision_mesh = getCollisionMesh(collision_mesh_name);
            }
        }

        extractTransformFromNode(node, n->transform.position, n->transform.rotation, n->transform.scale);

        if (mesh) {
            fw64_node_set_mesh(n, mesh);

            if (collision_mesh) {
                fw64_node_set_mesh_collider(n, scene->createCollider(), collision_mesh);
            }
            else {
                fw64_node_set_box_collider(n, scene->createCollider());
            }
        }

        fw64_node_update(n);
    }

    return scene;
}

fw64CollisionMesh* GlbParser::getCollisionMesh(std::string const & name) {
    auto result = scene->mesh_colliders.find(name);

    if (result != scene->mesh_colliders.end()) {
        return result->second.get();
    }

    int collider_mesh_index = findCollisionMeshIndex(name);

    assert(collider_mesh_index != -1);

    auto* collision_mesh = parseCollisionMesh(json_doc["meshes"][collider_mesh_index]);
    scene->mesh_colliders.emplace(std::make_pair(name, collision_mesh));

    return collision_mesh;
}

int GlbParser::findCollisionMeshIndex(std::string const& name){
    assert(collider_node_index != -1);

    auto const & collider_root_node = json_doc["nodes"][collider_node_index];
    assert(collider_root_node.contains("children"));

    int collider_node_mesh_index = -1;

    for (auto const & child : collider_root_node["children"]) {
        auto const & collider_node = json_doc["nodes"][child.get<int>()];

        if (!collider_node.contains("name"))
            continue;

        std::string collider_name = collider_node["name"].get<std::string>();

        if (collider_name != name)
            continue;

        if (!collider_node.contains("mesh"))
            continue;

        collider_node_mesh_index = collider_node["mesh"].get<int>();
        break;
    }

    return collider_node_mesh_index;
}

framework64::CollisionMesh* GlbParser::parseCollisionMesh(nlohmann::json const & mesh_node) {
    auto* collision_mesh = new framework64::CollisionMesh();

    auto const & primitive_node = mesh_node["primitives"][0];
    auto const & attributes_node = primitive_node["attributes"];
    auto accessor = attributes_node["POSITION"].get<size_t>();

    MeshData mesh_data = readPrimitiveMeshData(primitive_node);

    collision_mesh->point_data = std::move(mesh_data.positions);
    collision_mesh->element_data = std::move(mesh_data.indices_array_uint16);
    collision_mesh->element_count = static_cast<uint32_t>(collision_mesh->element_data.size());
    collision_mesh->point_count = static_cast<uint32_t>(collision_mesh->point_data.size()) / 3;
    collision_mesh->points = reinterpret_cast<Vec3*>(collision_mesh->point_data.data());
    collision_mesh->elements = collision_mesh->element_data.data();
    collision_mesh->box = getBoxFromAccessor(accessor);

    return collision_mesh;
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

void extractTransformFromNode(nlohmann::json const & node, Vec3& position, Quat& rotation, Vec3& scale) {
    vec3_zero(&position);
    quat_ident(&rotation);
    vec3_one(&scale);

    if (node.contains("translation")) {
        auto const & translation_node = node["translation"];
        position.x = translation_node[0].get<float>();
        position.y = translation_node[1].get<float>();
        position.z = translation_node[2].get<float>();
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
}

// note this assumes there is only one mesh at the top level
// it will need to be modified in the future
static std::array<float, 16> extractMatrixFromNode(nlohmann::json const & node) {
    std::array<float, 16> transform_matrix;
    matrix_set_identity(transform_matrix.data());

    Vec3 position, scale;
    Quat rotation;

    extractTransformFromNode(node, position, rotation, scale);

    bool set_trs = true;

    if (node.contains("matrix")) {
        auto const &matrix_node = node["matrix"];

        for (size_t i = 0; i < 16; i++) {
            transform_matrix[i] = matrix_node[i].get<float>();
        }

        set_trs = false;
    }

    if (set_trs) {
        matrix_from_trs(transform_matrix.data(), &position, &rotation, &scale);
    }

    return transform_matrix;
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

        MeshData mesh_data = readPrimitiveMeshData(primitive_node);

        if (!mesh_data.positions.empty()) {
            auto const & attributes_node = primitive_node["attributes"];
            auto accessor = attributes_node["POSITION"].get<size_t>();
            primitive.bounding_box = getBoxFromAccessor(accessor);
            box_encapsulate_box(&mesh->bounding_box, &primitive.bounding_box);
        }

        if (mesh_data.indices_array_uint16.empty() && mesh_data.indices_array_uint32.empty()) {
            return nullptr; // error?
        }

        GLMeshInfo gl_info = mesh_data.createMesh();
        gl_info.setPrimitiveValues(primitive);
        shader_cache.setShaderProgram(primitive);
    }

    return mesh;
}

MeshData GlbParser::readPrimitiveMeshData(nlohmann::json const & primitive_node) {
    MeshData mesh_data;

    auto const & attributes_node = primitive_node["attributes"];

    mesh_data.positions = readPrimitiveAttributeBuffer<float>(attributes_node, "POSITION");
    mesh_data.normals = readPrimitiveAttributeBuffer<float>(attributes_node, "NORMAL");
    mesh_data.tex_coords = readPrimitiveAttributeBuffer<float>(attributes_node, "TEXCOORD_0");
    mesh_data.colors = parseVertexColors(primitive_node);

    auto const & element_accessor_node = json_doc["accessors"][primitive_node["indices"].get<size_t>()];
    auto componentType = element_accessor_node["componentType"].get<uint32_t>();
    auto bufferViewIndex = element_accessor_node["bufferView"].get<size_t>();

    if (componentType == GltfComponentType::UnsignedShort) {
        mesh_data.indices_array_uint16 = readBufferViewData<uint16_t>(bufferViewIndex);
    }
    else if (componentType == GltfComponentType::UnsignedInt) {
        mesh_data.indices_array_uint32 = readBufferViewData<uint32_t >(bufferViewIndex);
    }

    return mesh_data;
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
