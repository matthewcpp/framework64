#include "framework64/desktop/glb_parser.h"

#include "framework64/desktop/scene.h"
#include "framework64/desktop/splitter.h"
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

fw64Mesh* GlbParser::loadStaticMesh(std::string const & path, JointMap const & joint_map) {
    if (!openFile(path))
        return nullptr;

    resetMaps();

    shared_resources = new SharedResources();
    auto* static_mesh = parseStaticMesh(json_doc["meshes"][0]);
    static_mesh->resources.reset(shared_resources);

    for (auto & primitive : static_mesh->primitives) {
        auto mapped_joint = joint_map.find(primitive.joint_index);

        if (mapped_joint != joint_map.end()) {
            primitive.joint_index = mapped_joint->second;
        }
    }

    return static_mesh;
}

void GlbParser::parseSceneNode(int rootNodeIndex) {
    scene_node_index = GLTF_INVALID_INDEX;
    collider_node_index = GLTF_INVALID_INDEX;

    auto const & node_arr = json_doc["nodes"][rootNodeIndex];

    if (!node_arr.contains("children"))
        return;

    auto const & children = node_arr["children"];

    for (auto const & node_index : children) {
        int index = node_index.get<int>();
        const auto & node = json_doc["nodes"][index];

        if (node.contains("name")) {
            std::string node_name = node["name"].get<std::string>();

            if (node_name.rfind("Scene", 0) == 0) {
                scene_node_index = index;
            }
            else if (node_name.rfind("Colliders", 0) == 0) {
                collider_node_index = index;
            }
        }
    }
}

// Precondition: fw64 node has already had its TRS extracted from the JSON node and json_node has children
void GlbParser::getCustomBoundingBoxForNode(nlohmann::json const & json_node, fw64Node* node) {
    for (auto const & child_index : json_node["children"]) {
        auto const & child_node = json_doc["nodes"][child_index.get<int>()];
        std::string name = child_node["name"].get<std::string>();

        if (name.find("Box") == -1)
            continue;

        Vec3 inv_scale = {1.0f / node->transform.scale.x, 1.0f / node->transform.scale.y, 1.0f / node->transform.scale.z};
        Vec3 localTranslation, localScale;
        Quat localRotation;

        extractTransformFromNode(child_node, localTranslation, localRotation, localScale);
        
        Vec3 extents = { inv_scale.x * localScale.x, inv_scale.y * localScale.y, inv_scale.z * localScale.z };

        Box custom_bounding;
        box_set_center_extents(&custom_bounding, &localTranslation, & extents);
        fw64_node_set_collider(node, scene->createCollider());
        fw64_collider_set_type_box(node->collider, &custom_bounding);

        break;
    }
}

fw64Scene* GlbParser::loadScene(std::string const & path, int rootNodeIndex, LayerMap const & layer_map) {
    if (path != file_path) {
        if (!openFile(path))
            return nullptr;

        file_path = path;
    }

    scene = new fw64Scene();
    shared_resources = &scene->shared_resources;
    resetMaps();

    parseSceneNode(rootNodeIndex);
    assert(scene_node_index != GLTF_INVALID_INDEX);

    // this node corresponds to the top level node in the level scene / chunk of which all scene nodes are children
    // not to be confused with a scene object in gltf
    auto const & scene_node = json_doc["nodes"][scene_node_index];
    if (!scene_node.contains("children")) // empty scene
        return scene;

    for (auto const & child_index : scene_node["children"]) {
        auto const & node = json_doc["nodes"][child_index.get<int>()];

        std::string name = node["name"].get<std::string>();

        auto* n = scene->createNode();
        fw64_node_init(n);

        extractTransformFromNode(node, n->transform.position, n->transform.rotation, n->transform.scale);

        bool skip_mesh_collider = false;

        if (node.contains("extras")) {
            auto& extras = node["extras"];

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

                if (collision_mesh_name == "none") {
                    skip_mesh_collider = true; // prevent the creation of any collider
                }
                else {
                    auto* collision_mesh = getCollisionMesh(collision_mesh_name);
                    fw64_node_set_collider(n, scene->createCollider());
                    fw64_collider_set_type_mesh(n->collider, collision_mesh);
                }
            }
        }

        if (node.contains("children")&& !skip_mesh_collider) {
            getCustomBoundingBoxForNode(node, n);
        }

        if (node.contains("mesh")) {
            size_t mesh_index = node["mesh"].get<size_t>();
            auto* mesh = getStaticMesh(mesh_index);
            fw64_node_set_mesh(n, mesh);

            if (!n->collider && !skip_mesh_collider) {
                fw64_node_set_collider(n, scene->createCollider());
                fw64_collider_set_type_box(n->collider, &mesh->bounding_box);
            }
        }

        fw64_node_update(n);
    }

    shared_resources = nullptr;
    return scene;
}

fw64CollisionMesh* GlbParser::getCollisionMesh(std::string const & name) {
    auto result = collisionMeshes.find(name);

    if (result != collisionMeshes.end()) {
        return result->second;
    }

    int collider_mesh_index = findCollisionMeshIndex(name);
    assert(collider_mesh_index != -1);

    auto* collision_mesh = parseCollisionMesh(json_doc["meshes"][collider_mesh_index]);
    collisionMeshes[name] = collision_mesh;
    scene->collision_meshes.emplace_back(collision_mesh);

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

bool GlbParser::openFile(std::string const& path) {
    glb_file.open(path, std::ios::binary);

    if (!glb_file)
        return false;

    if (!parseHeader() || !parseJsonChunk() || !parseBinaryChunk() )
        return false;

    return true;
}

void GlbParser::resetMaps() {
    gltfToTexture.clear();
    gltfToMesh.clear();
    gltfToImage.clear();
    collisionMeshes.clear();
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

fw64Mesh* GlbParser::parseStaticMesh(nlohmann::json const & node) {
    auto mesh = new fw64Mesh();

    for (auto const & primitive_node : node["primitives"]) {
        MeshData mesh_data = readPrimitiveMeshData(primitive_node);

        if (mesh_data.hasMultipleJointIndices()) {
            auto partitions = splitByJointIndex(mesh_data);

            for (auto & partition : partitions) {
                createPrimitive(mesh, primitive_node, partition);
            }
        }
        else {
            createPrimitive(mesh, primitive_node, mesh_data);
        }
    }

    return mesh;
}

void GlbParser::createPrimitive(fw64Mesh* mesh, nlohmann::json const & primitive_node, MeshData& mesh_data) {
    auto const primitive_mode = getPrimitiveMode(primitive_node);

    if (primitive_mode == fw64Primitive::Mode::Unknown)
        return;

    auto & primitive = mesh->primitives.emplace_back();

    primitive.mode = primitive_mode;

    if (!mesh_data.joint_indices.empty()) {
        primitive.joint_index = static_cast<uint32_t>(mesh_data.joint_indices[0]);
    }

    if (primitive_node.contains("material"))
        parseMaterial(primitive.material, primitive_node["material"].get<size_t>());

    if (!mesh_data.positions.empty()) {
        auto const & attributes_node = primitive_node["attributes"];
        auto accessor = attributes_node["POSITION"].get<size_t>();
        primitive.bounding_box = getBoxFromAccessor(accessor);
        box_encapsulate_box(&mesh->bounding_box, &primitive.bounding_box);
    }

    GLMeshInfo gl_info = mesh_data.createGlMesh();
    gl_info.setPrimitiveValues(primitive);
    mesh_data.moveMeshDataToPrimitive(primitive);
    determineShadingMode(primitive);
    primitive.material.shader = shader_cache.getShaderProgram(primitive.material.shading_mode);
}

// note that authoring vertex colors in the model gives priority over normals
void GlbParser::determineShadingMode(fw64Primitive& primitive) {
    if (primitive.material.shading_mode != FW64_SHADING_MODE_UNSET) // in this case the shading mode was explictly specified on material
        return;

    if (primitive.mode == fw64Primitive::Mode::Lines) {
        primitive.material.shading_mode = FW64_SHADING_MODE_LINE;
        return;
    }

    if (primitive.attributes & fw64Primitive::Attributes::VertexColors) {
        if (primitive.attributes & fw64Primitive::Attributes::TexCoords)
            primitive.material.shading_mode = FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED;
        else
            primitive.material.shading_mode = FW64_SHADING_MODE_VERTEX_COLOR;
        
        return;
    }

    if (primitive.attributes & fw64Primitive::Attributes::Normals) {
        primitive.material.shading_mode = primitive.material.texture ? FW64_SHADING_MODE_GOURAUD_TEXTURED : FW64_SHADING_MODE_GOURAUD;
        return;
    }
}

MeshData GlbParser::readPrimitiveMeshData(nlohmann::json const & primitive_node) {
    MeshData mesh_data;

    auto const & attributes_node = primitive_node["attributes"];

    mesh_data.positions = readPrimitiveAttributeBuffer<float>(attributes_node, "POSITION");
    mesh_data.normals = readPrimitiveAttributeBuffer<float>(attributes_node, "NORMAL");
    mesh_data.tex_coords = readPrimitiveAttributeBuffer<float>(attributes_node, "TEXCOORD_0");
    mesh_data.joint_indices = readPrimitiveAttributeBuffer<uint8_t>(attributes_node, "JOINTS_0"); // TODO: support other component types?
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

    assert (!mesh_data.indices_array_uint16.empty() || !mesh_data.indices_array_uint32.empty());

    return mesh_data;
}

static std::unordered_map<std::string, fw64ShadingMode> shading_mode_name_map = 
{
    {"UnlitTextured", FW64_SHADING_MODE_UNLIT_TEXTURED}
};

static fw64ShadingMode parseMaterialShadingMode(nlohmann::json const & material_extras_node) {
    if (!material_extras_node.contains("shadingMode"))
        return FW64_SHADING_MODE_UNSET;

    auto shading_mode_str = material_extras_node["shadingMode"].get<std::string>();
    auto shading_mode_lookup = shading_mode_name_map.find(shading_mode_str);

    if (shading_mode_lookup != shading_mode_name_map.end())
        return shading_mode_lookup->second;

    return FW64_SHADING_MODE_UNSET;
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

    if (material_node.contains("extras")) {
        auto extras = material_node["extras"];
        material.shading_mode = parseMaterialShadingMode(extras);
    }
}

fw64Texture* GlbParser::getTexture(size_t texture_index) {
    auto result = gltfToTexture.find(texture_index);
    if (result != gltfToTexture.end()) {
        return result->second;
    }

    auto* texture = parseTexture(texture_index);
    gltfToTexture[texture_index] = texture;

    if (shared_resources)
        shared_resources->textures.emplace_back(texture);

    return texture;
}

fw64Mesh* GlbParser::getStaticMesh(size_t mesh_index) {
    auto result = gltfToMesh.find(mesh_index);

    if (result != gltfToMesh.end()) {
        return result->second;
    }

    auto* mesh = parseStaticMesh(json_doc["meshes"][mesh_index]);
    gltfToMesh[mesh_index] = mesh;

    if (scene)
        scene->meshes.emplace_back(mesh);

    return mesh;
}

//TODO: read sampler parameters and apply to texture object
fw64Texture* GlbParser::parseTexture(size_t texture_index) {
    assert(gltfToTexture.count(texture_index) == 0);

    auto source_index = json_doc["textures"][texture_index]["source"].get<size_t>();

    auto* image = getImage(source_index);
    auto* texture =  new fw64Texture(image);

    // Note: default wrap mode for GLTF is repeat
    texture->wrap_t = FW64_TEXTURE_WRAP_REPEAT;
    texture->wrap_s = FW64_TEXTURE_WRAP_REPEAT;

    return texture;
}

fw64Image* GlbParser::getImage(size_t image_index) {
    auto result = gltfToImage.find(image_index);

    if (result != gltfToImage.end()) {
        return result->second;
    }

    auto* image = parseImage(image_index);
    gltfToImage[image_index] = image;

    if (shared_resources)
        shared_resources->images.emplace_back(image);

    return image;
}

fw64Image* GlbParser::parseImage(size_t image_index) {
    auto image_node = json_doc["images"][image_index];

    auto buffer_view_index = image_node["bufferView"].get<size_t>();
    auto image_data = readBufferViewData<uint8_t>(buffer_view_index);

    return fw64Image::loadImageBuffer(reinterpret_cast<void *>(image_data.data()), image_data.size());
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
