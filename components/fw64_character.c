#include "fw64_character.h"

#include "framework64/math.h"

void fw64_character_envionment_init(fw64CharacterEnvironment* env) {
    vec3_set(&env->gravity, 0.0f, Fw64_CHARACTER_ENV_DEFAULT_GRAVITY, 0.0f);
    env->max_substeps = FW64_CHARACTER_ENV_MAX_SUBSTEPS;
}

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene) {
    character->environment = env;
    character->scene = scene;

    Vec3 zero = vec3_zero();
    vw64_character_reset_position(character, &zero);

    vec3_set_one(&character->size);
    character->max_speed = FW64_CHARACTER_DEFAULT_MAX_SPEED;
}

void vw64_character_reset_position(fw64Character* character, const Vec3* position) {
    character->previous_position = *position;
    character->position = *position;
    character->state = FW64_CHARACTER_ON_GROUND;
    vec3_set_zero(&character->velocity);
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

        for (uint32_t t = 1; t < cell->floor_count; t++) {
            fw64CollisionTriangle* triangle = triangles + cell->floor_index + i;
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

    // determine sphere radius for querys
    float query_radius = fw64_maxf(character->size.x, character->size.z);

    // constrain character's speed (if necessary)
    const float max_step = query_radius * 0.5;
    float speed = vec3_length(&character->velocity);
    if (speed > character->max_speed) {
        vec3_normalize(&character->velocity);
        vec3_scale(&character->velocity, character->max_speed, &character->velocity);
        speed = character->max_speed;
    }

    // determine movement over the whole frame
    float move_distance = speed * time_delta;

    // determine how many substeps we will need to avoid tunneling
    int num_substeps = fw64_clampi((int)fw64_ceilf(move_distance / max_step), 1, character->environment->max_substeps);

    // update character's position over each substep
    float substep_dt = time_delta / (float)num_substeps;
    for (int i = 0; i < num_substeps; i++) {
        Vec3 step_move = character->velocity, query_pos, up = vec3_up();
        vec3_scale(&step_move, substep_dt, &step_move);
        vec3_add(&character->position, &step_move, &character->position);

        vec3_add_and_scale(&character->position, &up, query_radius, &query_pos);

        fw64CollisionGeometryQuery query;
        fw64_collision_geometry_query_vec3(character->scene->collision_geometry, &query_pos, &query);

        fw64_character_check_floor_collision(character, &query_pos, query_radius, &query);
    }
}
