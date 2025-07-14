#include "fw64_character.h"

#include "framework64/math.h"

void fw64_character_envionment_init(fw64CharacterEnvironment* env) {
    vec3_set(&env->gravity, 0.0f, Fw64_CHARACTER_ENV_DEFAULT_GRAVITY, 0.0f);
    env->max_fall_speed = FW64_CHARACTER_ENV_MAX_FALL_SPEED;
    env->max_substeps = FW64_CHARACTER_ENV_MAX_SUBSTEPS;
    env->horizontal_move_threshold = FW64_CHARACTER_ENV_HORIZ_MOVE_THRESHOLD;
}

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene) {
    character->environment = env;
    character->scene = scene;

    Vec3 zero = vec3_zero();
    character->attempt_to_move = zero;
    fw64_character_reset_position(character, &zero);

    vec3_set_one(&character->size);
    character->max_speed = FW64_CHARACTER_DEFAULT_MAX_SPEED;
    character->jump_speed = FW64_CHARACTER_DEFAULT_JUMP_SPEED;
    character->jump_fall_gravity_scale = FW64_CHARACTER_DEFAULT_JUMP_FALL_GRAVITY_SCALE;
    character->gravity_scale = FW64_CHARACTER_DEFAULT_GRAVITY_SCALE;

    character->attempt_to_jump = 0;
}

void fw64_character_reset_position(fw64Character* character, const Vec3* position) {
    character->previous_position = *position;
    character->position = *position;
    character->previous_state = FW64_CHARACTER_STATE_IN_AIR;
    character->state = FW64_CHARACTER_STATE_IN_AIR;
    vec3_set_zero(&character->velocity);
}

static void fw64_character_check_floor_collision(fw64Character* character, const Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    // check floor collisions
    // TODO: https://brendankeesing.com/blog/character_controller_stairs/
    
    Vec3 hit_point, correction_vector = vec3_zero(), query_v0;
    int hit_count = 0;
    float total_penetration = 0.0f;

    for (int c = 0; c < query->cell_count; c++) {
        fw64CollisionGeometryCell* cell = query->cells[c];
        fw64CollisionTriangle* triangles = character->scene->collision_geometry->triangles + cell->floor_index;

        for (uint32_t t = 0; t < cell->floor_count; t++) {
            fw64CollisionTriangle* triangle = triangles + t;

            // initial filter: check penetration with triangle plane
            vec3_subtract(query_pos, &triangle->A, &query_v0);
            float distance = vec3_dot(&triangle->N, &query_v0);
            if (distance < query_radius) {
                // precision check
                if (fw64_collision_test_sphere_triangle(query_pos, query_radius, &triangle->A, &triangle->B, &triangle->C, &hit_point)) {
                    // correct position along collision normal
                    float penetration = query_radius - distance;
                    vec3_add_and_scale(&correction_vector, &triangle->N, penetration, &correction_vector);
                    hit_count += 1;
                    total_penetration += penetration;
                }
            }
        }
    }

    // resolve all collisions
    if (hit_count) {
        vec3_normalize(&correction_vector);
        vec3_scale(&correction_vector, total_penetration / (float)hit_count, &correction_vector);
        vec3_add(&character->position, &correction_vector, &character->position);

        //attempt to prevent jittering by skipping slight movement that may arise due to floating point effects
        // if (vec3_distance_squared(&character->previous_position, &character->position) < character->environment->horizontal_move_threshold) {
        //     character->position = character->previous_position;
        // }

        character->velocity.y = 0.0f;
        character->state = FW64_CHARACTER_STATE_ON_GROUND;
    } else {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
    }
}

void fw64_character_fixed_update(fw64Character* character, float time_delta) {
    character->previous_state = character->state;
    character->previous_position = character->position;

    // handle horizontal movement
    // TODO: handle air movement slightly differently?
    vec3_scale(&character->attempt_to_move, character->max_speed, &character->attempt_to_move);
    character->velocity.x = character->attempt_to_move.x;
    character->velocity.z = character->attempt_to_move.z;
    vec3_set_zero(&character->attempt_to_move);

    // handle jumping
    if (character->attempt_to_jump && fw64_character_is_on_ground(character)) {
        character->velocity.y += character->jump_speed;
        character->state = FW64_CHARACTER_STATE_IN_AIR;
        character->attempt_to_jump = 0;
    }

    // apply gravity to character's velocity
    Vec3 gravity = character->environment->gravity;
    vec3_scale(&gravity, character->gravity_scale, &gravity);
    if (character->velocity.y < 0.0f) {
        vec3_scale(&gravity, character->jump_fall_gravity_scale, &gravity);
    }
    vec3_add_and_scale(&character->velocity, &gravity, time_delta, &character->velocity);
    if (character->velocity.y < 0.0f) {
        character->velocity.y = fw64_maxf(character->velocity.y, character->environment->max_fall_speed);
    }

    // constrain character's horizontal speed (if necessary)
    Vec2 horizontal_movement = {character->velocity.x, character->velocity.z};
    float speed = vec2_length(&horizontal_movement);

    if (speed > character->max_speed) {
        speed = character->max_speed;

        // update horizontal movement components of velocity (preserve vertical component)
        vec2_normalize(&horizontal_movement);
        character->velocity.x = horizontal_movement.x * speed;
        character->velocity.z = horizontal_movement.y * speed;
    }

    // determine movement over the whole frame
    float move_distance = speed * time_delta;

    // determine sphere radius for queries
    // this will also be used for max step calculation to help prevent tunneling.
    float query_radius = fw64_maxf(character->size.x, character->size.z);

    // determine how many substeps we will need to avoid tunneling
    const float max_step = query_radius * 0.5;
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
