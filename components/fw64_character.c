#include "fw64_character.h"

#include <framework64/log.h>
#include "framework64/math.h"
#include <framework64/types.h>

void fw64_character_envionment_init(fw64CharacterEnvironment* env) {
    vec3_set(&env->gravity, 0.0f, Fw64_CHARACTER_ENV_DEFAULT_GRAVITY, 0.0f);
    env->max_fall_speed = FW64_CHARACTER_ENV_MAX_FALL_SPEED;
    env->max_substeps = FW64_CHARACTER_ENV_MAX_SUBSTEPS;
    env->horizontal_move_threshold = FW64_CHARACTER_ENV_HORIZ_MOVE_THRESHOLD;
}

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Node* node, fw64Scene* scene) {
    character->environment = env;
    character->node = node;
    character->scene = scene;

    Vec3 zero = vec3_zero();
    character->attempt_to_move = zero;
    fw64_character_set_position(character, &zero);

    vec3_set_one(&character->size);
    character->head_height = 0.8f;
    character->hang_vertical_offset = 0.0f;
    character->max_speed = FW64_CHARACTER_DEFAULT_MAX_SPEED;
    character->jump_speed = FW64_CHARACTER_DEFAULT_JUMP_SPEED;
    character->jump_fall_gravity_scale = FW64_CHARACTER_DEFAULT_JUMP_FALL_GRAVITY_SCALE;
    character->gravity_scale = FW64_CHARACTER_DEFAULT_GRAVITY_SCALE;

    character->ground_accel = FW64_CHARACTER_DEFAULT_GROUND_ACCEL;
    character->ground_decel = FW64_CHARACTER_DEFAULT_GROUND_DECEL;

    character->attempt_to_jump = 0;
}

void fw64_character_set_position(fw64Character* character, const Vec3* position) {
    character->previous_position = *position;
    character->position = *position;
    character->previous_state = FW64_CHARACTER_STATE_IN_AIR;
    character->state = FW64_CHARACTER_STATE_IN_AIR;
    vec3_set_zero(&character->velocity);
}

static int _fw64_character_attempt_sticky_ground(fw64Character* character, const Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    float closest_t = FLT_MAX;
    const Vec3 ray_origin = {query_pos->x, query_pos->y + query_radius, query_pos->z};
    const Vec3 ray_dir = vec3_down();
    Vec3 sticky_pos = vec3_zero();
    
    for (uint32_t c = 0; c < query->cell_count; c++) {
        fw64CollisionGeometryCell* cell = query->cells[c];
        fw64CollisionTriangle* triangles = character->scene->collision_geometry->triangles + cell->floor_index;

        for (uint32_t t = 0; t < cell->floor_count; t++) {
            fw64CollisionTriangle* triangle = triangles + t;
            float current_t;
            Vec3 ray_pos;

            if (fw64_collision_test_ray_triangle(&ray_origin, &ray_dir, &triangle->A, &triangle->B, &triangle->C, &ray_pos, &current_t)) {
                if (current_t < closest_t) {
                    closest_t = current_t;
                    sticky_pos = ray_pos;
                }
            }
        }
    }

    if (closest_t == FLT_MAX) {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
        return 0;
    }

    float y_delta = fw64_fabsf(character->position.y - sticky_pos.y);

    // sticky ground threshold
    if (y_delta <= 0.07) {
        character->position = sticky_pos;
        character->state = FW64_CHARACTER_STATE_ON_GROUND;
        return 1;
    } else {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
        return 0;
    }
}

static void fw64_character_check_floor_collision(fw64Character* character, const Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    // check floor collisions
    // TODO: https://brendankeesing.com/blog/character_controller_stairs/
    
    Vec3 hit_point, correction_vector = vec3_zero(), query_v0;
    int hit_count = 0;
    float total_penetration = 0.0f;

    for (uint32_t c = 0; c < query->cell_count; c++) {
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

        // attempt to prevent jittering by skipping slight movement that may arise due to floating point effects
        if (vec3_distance_squared(&character->previous_position, &character->position) < character->environment->horizontal_move_threshold
            && !(fw64_character_is_moving_horizontally(character))) {
            character->position = character->previous_position;
            vec3_set_zero(&character->velocity);
        }

        character->state = FW64_CHARACTER_STATE_ON_GROUND;
    } else {
        // if we were on the ground and not jumping, we would like to attempt to stick to the ground
        // if we are close to a ground triangle.  This should help prevent the case where we are running slightly
        // faster than gravity can pull us down
        if (fw64_character_is_on_ground(character) &&  character->velocity.y <= 0.0f) {
            _fw64_character_attempt_sticky_ground(character, query_pos, query_radius, query);
        } else {
            character->state = FW64_CHARACTER_STATE_IN_AIR;
        }
    }

    if (fw64_character_is_on_ground(character)) {
        character->velocity.y = 0.0f;
    }
}


static void fw64_character_compute_ledge_check_origin(fw64Character* character, Vec3* ledge_check_origin) {
    const float grab_size = 0.3f; // TODO: is this right?

    *ledge_check_origin = character->position;
    ledge_check_origin->y += character->size.y + 0.3f;


    Vec3 ledge_check_offset;
    fw64_transform_back(&character->node->transform, &ledge_check_offset); // flip should be forward
    vec3_scale(&ledge_check_offset, grab_size, &ledge_check_offset);
    vec3_add(ledge_check_origin, &ledge_check_offset, ledge_check_origin);
}

#define fw64_character_get_query_radius(character) (fw64_maxf((character)->size.x, (character)->size.z))

void fw64_character_get_ledge_check_origin(fw64Character* character, Vec3* out) {
    fw64_character_compute_ledge_check_origin(character, out);
}

// TODO: should this go into collision geometry?
static fw64CollisionTriangle* fw64_character_get_closest_triangle_for_ray(fw64Character* character, const Vec3* origin, const Vec3* direction, float maxt_t, fw64CollisionGeometryType geometry_type, Vec3* closest_pt) {
    fw64CollisionGeometryQuery query;
    if (!fw64_collision_geometry_query_vec3(character->scene->collision_geometry, origin, &query)) {
        return 0;
    }

    float closest_t = maxt_t, current_t;
    Vec3 current_pt;
    fw64CollisionTriangle* closest_triangle = NULL;

    for (uint32_t c = 0; c < query.cell_count; c++) {
        fw64CollisionGeometryCell* cell = query.cells[c];
        uint32_t triangle_index = 0;
        uint32_t triangle_count = 0;

        switch (geometry_type)
        {
            case FW64_COLLISION_GEOMETRY_TYPE_FLOOR:
                triangle_index = cell->floor_index;
                triangle_count = cell->floor_count;
                break;
        
            case FW64_COLLISION_GEOMETRY_TYPE_WALL:
                triangle_index = cell->wall_index;
                triangle_count = cell->wall_count;
                break;

            case FW64_COLLISION_GEOMETRY_TYPE_CEILING:
                triangle_index = cell->ceiling_index;
                triangle_count = cell->ceiling_count;
                break;
        }

        fw64CollisionTriangle* triangles = character->scene->collision_geometry->triangles + triangle_index;

        for (uint32_t t = 0; t < triangle_count; t++) {
            fw64CollisionTriangle* triangle = triangles + t;

            if (fw64_collision_test_ray_triangle(origin, direction, &triangle->A, &triangle->B, &triangle->C, &current_pt, &current_t)) {
                if (current_t < closest_t) {
                    closest_t = current_t;
                    *closest_pt = current_pt;
                    closest_triangle = triangle;
                }
            }
        }
    }

    return closest_triangle;
}

/**
 * Ledge grab consists of two raycasts
 * First raycast shoots forward from the character looking for walls
 * If one is found then run the grab collison check
 */
int fw64_character_attempt_ledge_grab(fw64Character* character, float query_radius) {
    Vec3 ledge_wall_check_origin = character->position, ledge_wall_check_direction, closest_pt;
    ledge_wall_check_origin.y += character->head_height;
    fw64_transform_back(&character->node->transform, &ledge_wall_check_direction); // todo flip vector

    const float ledge_wall_check_max_dist = query_radius * 1.1f;
    fw64CollisionTriangle* wall_triangle = fw64_character_get_closest_triangle_for_ray(character, &ledge_wall_check_origin, &ledge_wall_check_direction, ledge_wall_check_max_dist, FW64_COLLISION_GEOMETRY_TYPE_WALL, &closest_pt);
    if (!wall_triangle) {
        return 0;
    }

    Vec3 ledge_check_origin, ledge_check_dir = vec3_down();
    fw64_character_compute_ledge_check_origin(character, &ledge_check_origin);

    if (!fw64_character_get_closest_triangle_for_ray(character, &ledge_check_origin, &ledge_check_dir, FLT_MAX, FW64_COLLISION_GEOMETRY_TYPE_FLOOR, &closest_pt)) {
        return 0;
    }

    if ((vec3_distance_squared(&ledge_check_origin, &closest_pt) <= query_radius * query_radius)) {
        character->state = FW64_CHARACTER_STATE_LEDGE_HANG;
        vec3_set_zero(&character->velocity);

        // we want to push the character out from the collision point
        vec3_add_and_scale(&character->position, &wall_triangle->N, query_radius, &character->position);
        character->position.y = closest_pt.y - character->size.y + character->hang_vertical_offset;

        // rotate the character to face the wall
        Vec3 character_forward = {wall_triangle->N.x, 0.0f, wall_triangle->N.z};
        vec3_normalize(&character_forward);
        vec3_negate(&character_forward);
        float yaw = atan2f(character_forward.x, character_forward.z);
        quat_set_axis_angle(&character->node->transform.rotation, 0.0f, 1.0f, 0.0f, yaw);
        fw64_node_update(character->node);

        return 1;
    }

    return 0;
}

void fw64_character_drop_from_ledge(fw64Character* character) {
    // push character back from ledge
    Vec3 back, fall_pos;
    fw64_transform_forward(&character->node->transform, &back); // todo flip this fuc
    vec3_scale(&back, fw64_character_get_query_radius(character), &back);
    vec3_add(&character->position, &back, &fall_pos);
    fw64_character_set_position(character, &fall_pos);
}

void fw64_character_start_climbing_up_ledge(fw64Character* character) {
    character->state = FW64_CHARACTER_STATE_LEDGE_CLIMB_UP;
}

void fw64_character_finish_climbing_up_ledge(fw64Character* character, const Vec3* new_pos) {
    fw64_character_set_position(character, new_pos);
}

static void fw64_character_check_wall_collision(fw64Character* character, const Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    Vec3 hit_point, correction_vector = vec3_zero(), query_v0;
    int hit_count = 0;
    float total_penetration = 0.0f;

    for (uint32_t c = 0; c < query->cell_count; c++) {
        fw64CollisionGeometryCell* cell = query->cells[c];
        fw64CollisionTriangle* triangles = character->scene->collision_geometry->triangles + cell->wall_index;

        for (uint32_t t = 0; t < cell->wall_count; t++) {
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

        character->velocity.x = 0.0f;
        character->velocity.z = 0.0f;
    }
}

void fw64_character_fixed_update(fw64Character* character, float time_delta) {
    character->previous_state = character->state;
    character->previous_position = character->position;

    if (!fw64_character_is_enabled(character) || fw64_character_is_interacting_with_ledge(character)) {
        return;
    }

    // handle horizontal movement with acceleration
    Vec3 current_ground_velocity = {character->velocity.x, 0.0f, character->velocity.z};
    float ground_speed = vec3_length(&current_ground_velocity);
    float desired_speed = fw64_character_is_attempting_to_move(character) ? character->max_speed : 0.0f;

    // if player is accelerating, then move them the direction they are attempting to move
    if (desired_speed >= ground_speed) {
        ground_speed += character->ground_accel * time_delta;
        current_ground_velocity = character->attempt_to_move;
    } else if (desired_speed < ground_speed) {
        ground_speed -= character->ground_decel * time_delta;
    }

    ground_speed = fw64_clamp(ground_speed, 0.0f, character->max_speed);
    vec3_normalize(&current_ground_velocity);
    vec3_scale(&current_ground_velocity, ground_speed, &current_ground_velocity);
    character->velocity.x = current_ground_velocity.x;
    character->velocity.z = current_ground_velocity.z;

    // handle jumping
    if (character->attempt_to_jump && fw64_character_is_on_ground(character)) {
        character->velocity.y += character->jump_speed;
        character->state = FW64_CHARACTER_STATE_IN_AIR;
    }

    vec3_set_zero(&character->attempt_to_move);
    character->attempt_to_jump = 0;

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
    float query_radius = fw64_character_get_query_radius(character);

    // determine how many substeps we will need to avoid tunneling
    const float max_step = query_radius * 0.5f;
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

        const int did_grab_ledge = !fw64_character_is_on_ground(character) && fw64_character_attempt_ledge_grab(character, query_radius);

        if (!did_grab_ledge) {
            vec3_add_and_scale(&character->position, &up, query_radius, &query_pos);
            fw64_character_check_wall_collision(character, &query_pos, query_radius, &query);
        }
    }
}
