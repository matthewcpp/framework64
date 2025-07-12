#include "fw64_character.h"

#include "framework64/math.h"

void fw64_character_envionment_init(fw64CharacterEnvironment* env) {
    vec3_set(&env->gravity, 0.0f, Fw64_CHARACTER_ENV_DEFAULT_GRAVITY, 0.0f);
}

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene) {
    character->environment = env;
    character->scene = scene;

    character->state = FW64_CHARACTER_ON_GROUND;
    vec3_set_zero(&character->velocity);

    vec3_set_zero(&character->previous_position);
    vec3_set_zero(&character->position);
    vec3_set_one(&character->size);
    character->step_height = 0.1f;
}

static void fw64_character_check_floor_collision(fw64Character* character, const Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    if (character->velocity.y > 0.0f) {
        character->state = FW64_CHARACTER_IN_AIR;
        return;
    }

    // check floor collisions
    // TODO: https://brendankeesing.com/blog/character_controller_stairs/
    for (int i = 0; i < query->cell_count; i++) {
        fw64CollisionGeometryCell* cell = query->cells[i];
        fw64CollisionTriangle* triangles = character->scene->collision_geometry->triangles + cell->floor_index;

        Vec3 hit_point;

        for (uint32_t t = 0; t < cell->floor_count; t++) {
            fw64CollisionTriangle* triangle = triangles + i;
            if (fw64_collision_test_sphere_triangle(query_pos, query_radius, &triangle->A, &triangle->B, &triangle->C, &hit_point)) {
                character->position = hit_point;
                character->velocity.y = 0.0f;
                character->state = FW64_CHARACTER_ON_GROUND;

                return;
            }
        }
    }

    character->state = FW64_CHARACTER_IN_AIR;
}

void fw64_character_fixed_update(fw64Character* character, float time_delta) {
    character->previous_position = character->position;

    // apply gravity to character's velocity
    vec3_add_and_scale(&character->velocity, &character->environment->gravity, time_delta, &character->velocity);

    // calculate the character's next position
    Vec3 movement = character->velocity;
    vec3_scale(&movement, time_delta, &movement);
    vec3_add(&character->position, &movement, &character->position);

    fw64CollisionGeometryQuery query;
    fw64_collision_geometry_query_vec3(character->scene->collision_geometry, &character->position, &query);

    float query_radius = fw64_maxf(character->size.x, character->size.z);
    Vec3 query_pos, up = vec3_up();
    vec3_add_and_scale(&character->position, &up, query_radius, &query_pos);

    fw64_character_check_floor_collision(character, &query_pos, query_radius, &query);
}
