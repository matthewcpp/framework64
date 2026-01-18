#include "fw64_character.h"

#include <framework64/log.h>
#include <framework64/math.h>
#include <framework64/types.h>

#include <string.h>

void fw64_character_envionment_init(fw64CharacterEnvironment* env) {
    vec3_set(&env->gravity, 0.0f, Fw64_CHARACTER_ENV_DEFAULT_GRAVITY, 0.0f);
    env->max_fall_speed = FW64_CHARACTER_ENV_MAX_FALL_SPEED;
    env->max_substeps = FW64_CHARACTER_ENV_MAX_SUBSTEPS;
    env->horizontal_move_threshold = FW64_CHARACTER_ENV_HORIZ_MOVE_THRESHOLD;
}

void _fw64_character_environment_debug_info_reset(fw64CharacterEnvironmentDebugInfo* debug) {
    memset(debug, 0, sizeof(fw64CharacterEnvironmentDebugInfo));
}

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Node* node, fw64Scene* scene) {
    character->environment = env;
    character->node = node;
    character->scene = scene;

    Vec3 zero = vec3_zero(), one = vec3_one();
    character->attempt_to_move = zero;
    fw64_character_set_position(character, &zero);
    fw64_character_set_size(character, &one);
    
    character->sphere_query_radius = 0.5f;
    character->step_height = 0.17;
    character->head_height = 0.8f;
    character->hang_vertical_offset = 0.0f;
    character->max_speed = FW64_CHARACTER_DEFAULT_MAX_SPEED;
    character->jump_speed = FW64_CHARACTER_DEFAULT_JUMP_SPEED;
    character->ladder_climb_speed = FW64_CHARACTER_DEFAULT_LADDER_CLIMB_SPEED;
    character->ladder_exit_height_adjustment = FW64_CHARACTER_DEFAULT_LADDER_HEIGHT_ADJUSTMENT;
    character->jump_fall_gravity_scale = FW64_CHARACTER_DEFAULT_JUMP_FALL_GRAVITY_SCALE;
    character->gravity_scale = FW64_CHARACTER_DEFAULT_GRAVITY_SCALE;

    character->ground_accel = FW64_CHARACTER_DEFAULT_GROUND_ACCEL;
    character->ground_decel = FW64_CHARACTER_DEFAULT_GROUND_DECEL;

    character->attempt_to_jump = 0;
}

static void fw64_character_update_capsule(fw64Character* character) {
    Vec3* base = &character->position;
    Vec3 tip = character->position;
    tip.y += character->size.y;

    fw64_capsule_set_points(&character->capsule, base, &tip);
}

void fw64_character_set_position(fw64Character* character, const Vec3* position) {
    character->previous_position = *position;
    character->position = *position;
    character->previous_state = FW64_CHARACTER_STATE_IN_AIR;
    character->state = FW64_CHARACTER_STATE_IN_AIR;
    vec3_set_zero(&character->velocity);
    character->active_ladder = NULL;

    fw64_character_update_capsule(character);
}

void fw64_character_set_size(fw64Character* character, const Vec3* size) {
    character->size = *size;

    float radius = fw64_maxf(size->x, size->z);
    Vec3 tip = character->position;
    tip.y += size->y;
    fw64_capsule_init(&character->capsule, &character->position, &tip, radius);
}

/** TODO: can this be reduced to use collision geometry raycast? */
static int _fw64_character_attempt_sticky_ground(fw64Character* character, const Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    float closest_t = FLT_MAX;
    const Vec3 ray_origin = {query_pos->x, query_pos->y + query_radius, query_pos->z};
    const Vec3 ray_dir = vec3_down();
    Vec3 sticky_pos = vec3_zero();

    Vec3 box_pt;
    float box_t;
    
    for (uint32_t c = 0; c < query->cell_count; c++) {
        fw64CollisionGeometryCell* cell = query->cells[c];

        for (uint16_t i = 0; i < cell->bounding_volume_count; i++) {
            const fw64CollisionGeometryBoundingVolume* bounding_volume = character->scene->collision_geometry->bounding_volumes + cell->bounding_volume_index + i;

            if (!fw64_collision_test_ray_box(&ray_origin, &ray_dir, &bounding_volume->primitive, &box_pt, &box_t)) {
                continue;
            }

            fw64CollisionTriangle* triangles = character->scene->collision_geometry->triangles + bounding_volume->floor_index;
            fw64_character_environment_increment_ray_triangles_checked(&character->environment->debug_info, bounding_volume->floor_count);

            for (uint32_t t = 0; t < bounding_volume->floor_count; t++) {
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
    }

    if (closest_t == FLT_MAX) {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
        return 0;
    }

    const float y_delta = fw64_fabsf(character->position.y - sticky_pos.y);
    const float sticky_dist = 0.35f * character->step_height;

    // sticky ground threshold
    if (y_delta <= sticky_dist) {
        character->position = sticky_pos;
        character->state = FW64_CHARACTER_STATE_ON_GROUND;
        return 1;
    } else {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
        return 0;
    }
}

static fw64CollisionTriangle* _get_bounding_volume_floors(const fw64CollisionGeometry* collision_geometry, const fw64CollisionGeometryBoundingVolume* bounding_volume, uint16_t* count) {
    *count = bounding_volume->floor_count;
    return collision_geometry->triangles + bounding_volume->floor_index;
}

static void _resolve_triangle_collision(fw64Character* character, int substep, Vec3* query_pos, float query_radius, fw64CollisionTriangle* triangle, const Vec3* hit_point, float collision_plane_dist) {
    (void)substep;
    (void)hit_point;
    Vec3 correction_vector;
    // correct position along collision normal
    float penetration = query_radius - collision_plane_dist;
    vec3_scale(&triangle->N, penetration, &correction_vector);
    vec3_add(&character->position, &correction_vector, &character->position);
    vec3_add(query_pos, &correction_vector, query_pos);
}

static void _resolve_wall_collision(fw64Character* character, int substep, Vec3* query_pos, float query_radius, fw64CollisionTriangle* triangle, const Vec3* hit_point, float collision_plane_dist) {
    if (hit_point->y - character->position.y <= character->step_height) {
        fw64_log_message("attempt step up: %f (%f)", (hit_point->y - character->position.y), character->step_height);
    }
    _resolve_triangle_collision(character, substep, query_pos, query_radius, triangle, hit_point, collision_plane_dist);
}

static fw64CollisionTriangle* _get_bounding_volume_walls(const fw64CollisionGeometry* collision_geometry, const fw64CollisionGeometryBoundingVolume* bounding_volume, uint16_t* count) {
    *count = bounding_volume->wall_count;
    return collision_geometry->triangles + bounding_volume->wall_index;
}

typedef fw64CollisionTriangle* (*_GetBoundingVolumeTriangleFunc)(const fw64CollisionGeometry* collision_geometry, const fw64CollisionGeometryBoundingVolume* bounding_volume, uint16_t* count);
typedef void (*_ResolveTriangleCollisionFunc)(fw64Character* character, int substep, Vec3* query_pos, float query_radius, fw64CollisionTriangle* triangle, const Vec3* hit_point, float collision_plane_dist);

static int fw64_character_check_sphere_collision(fw64Character* character, int substep, Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query, _GetBoundingVolumeTriangleFunc get_bounding_volume_triangles, _ResolveTriangleCollisionFunc resolve_func) {    
    Vec3 hit_point, query_v0;
    const float query_min = query_pos->y - query_radius;
    const float query_max = query_pos->y + query_radius;
    uint16_t triangle_count;
    int hit_count = 0;

    for (uint32_t c = 0; c < query->cell_count; c++) {
        fw64CollisionGeometryCell* cell = query->cells[c];

        for (uint16_t i = 0; i < cell->bounding_volume_count; i++) {
            const fw64CollisionGeometryBoundingVolume* bounding_volume = character->scene->collision_geometry->bounding_volumes + cell->bounding_volume_index + i;
            fw64CollisionTriangle* triangles = get_bounding_volume_triangles(character->scene->collision_geometry, bounding_volume, &triangle_count);
            fw64_character_environment_increment_sphere_triangles_considered(&character->environment->debug_info, triangle_count);

            if (!fw64_collision_test_box_sphere(&bounding_volume->primitive, query_pos, query_radius, &hit_point)) {
                fw64_character_environment_increment_sphere_triangles_skipped(&character->environment->debug_info, triangle_count);
                continue;
            }

            for (uint32_t t = 0; t < triangle_count; t++) {
                fw64CollisionTriangle* triangle = triangles + t;

                // filter triangles that are vertically outside of our query radius
                if (query_min > triangle->maxY || query_max < triangle->minY) {
                    fw64_character_environment_increment_sphere_triangles_skipped(&character->environment->debug_info, 1);
                    continue;
                }

                // check penetration with triangle plane
                vec3_subtract(query_pos, &triangle->A, &query_v0);
                const float distance = vec3_dot(&triangle->N, &query_v0);
                if (distance < query_radius) {
                    fw64_character_environment_increment_sphere_triangles_checked(&character->environment->debug_info, 1);

                    // precision check
                    if (fw64_collision_test_sphere_triangle(query_pos, query_radius, &triangle->A, &triangle->B, &triangle->C, &hit_point)) {
                        resolve_func(character, substep, query_pos, query_radius, triangle, &hit_point, distance);
                        hit_count += 1;
                    }
                }
            }
        }
    }

    return hit_count;
}

// TODO: https://brendankeesing.com/blog/character_controller_stairs/
static void fw64_character_check_floor_collision(fw64Character* character, int substep, Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    int hit_count = fw64_character_check_sphere_collision(character, substep, query_pos, query_radius, query, _get_bounding_volume_floors, _resolve_triangle_collision);

    // resolve all collisions
    if (hit_count > 0) {
        // attempt to prevent jittering by skipping slight movement that may arise due to floating point effects
        if (vec3_distance_squared(&character->previous_position, &character->position) < character->environment->horizontal_move_threshold
            && !(fw64_character_is_moving_horizontally(character))) {
            character->position = character->previous_position;
            vec3_set_zero(&character->velocity);
        }

        character->state = FW64_CHARACTER_STATE_ON_GROUND;
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

void fw64_character_get_ledge_check_origin(fw64Character* character, Vec3* out) {
    fw64_character_compute_ledge_check_origin(character, out);
}

/** Updates the character's node such that they are looking in the direction of the supplied normal
 * Used to snap the character to the wall when ledge hanging or to the ladder when climbing.
 */
static void fw64_character_rotate_to_face_normal(fw64Character* character, const Vec3* normal) {
    Vec3 character_forward = {normal->x, 0.0f, normal->z};
    vec3_normalize(&character_forward);
    vec3_negate(&character_forward);
    float yaw = atan2f(character_forward.x, character_forward.z);
    quat_set_axis_angle(&character->node->transform.rotation, 0.0f, 1.0f, 0.0f, yaw);
    fw64_node_update(character->node);
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
    const fw64CollisionTriangle* wall_triangle = fw64_collision_geometry_raycast_triangle(character->scene->collision_geometry, &ledge_wall_check_origin, &ledge_wall_check_direction, -FLT_MAX, ledge_wall_check_max_dist, FW64_COLLISION_GEOMETRY_TYPE_WALL, &closest_pt);
    if (!wall_triangle) {
        return 0;
    }

    Vec3 ledge_check_origin, ledge_check_dir = vec3_down();
    fw64_character_compute_ledge_check_origin(character, &ledge_check_origin);

    if (!fw64_collision_geometry_raycast_triangle(character->scene->collision_geometry, &ledge_check_origin, &ledge_check_dir, -FLT_MAX, FLT_MAX, FW64_COLLISION_GEOMETRY_TYPE_FLOOR, &closest_pt)) {
        return 0;
    }

    if ((vec3_distance_squared(&ledge_check_origin, &closest_pt) <= query_radius * query_radius)) {
        character->state = FW64_CHARACTER_STATE_LEDGE_HANG;
        vec3_set_zero(&character->velocity);

        // we want to push the character out from the collision point
        vec3_add_and_scale(&character->position, &wall_triangle->N, query_radius, &character->position);
        character->position.y = closest_pt.y - character->size.y + character->hang_vertical_offset;

        // rotate the character to face the wall
        fw64_character_rotate_to_face_normal(character, &wall_triangle->N);

        return 1;
    }

    return 0;
}

void fw64_character_drop_from_ledge(fw64Character* character) {
    // push character back from ledge
    Vec3 back, fall_pos;
    fw64_transform_forward(&character->node->transform, &back); // todo flip this fuc
    vec3_scale(&back, character->sphere_query_radius, &back);
    vec3_add(&character->position, &back, &fall_pos);
    fw64_character_set_position(character, &fall_pos);
}

void fw64_character_start_climbing_up_ledge(fw64Character* character) {
    character->state = FW64_CHARACTER_STATE_LEDGE_CLIMB_UP;
}

void fw64_character_finish_climbing_up_ledge(fw64Character* character, const Vec3* new_pos) {
    fw64_character_set_position(character, new_pos);
}

void fw64_character_finish_exiting_ladder(fw64Character* character, const Vec3* new_pos){
    character->active_ladder = NULL;
    fw64_character_set_position(character, new_pos);
}

void fw64_character_finish_entering_ladder(fw64Character* character, const Vec3* new_pos) {
    fw64_character_set_position(character, new_pos);
}

static void fw64_character_check_wall_collision(fw64Character* character, int substep, Vec3* query_pos, float query_radius, fw64CollisionGeometryQuery* query) {
    int hit_count = fw64_character_check_sphere_collision(character, substep, query_pos, query_radius, query, _get_bounding_volume_walls, _resolve_wall_collision);

    // resolve all collisions
    if (hit_count > 0) {
        character->velocity.x = 0.0f;
        character->velocity.z = 0.0f;
    }
}

static int fw64_character_attempt_ladder_grab(fw64Character* character, float query_radius) {
    fw64CollisionGeometryQuery query;
    if (!fw64_collision_geometry_query_vec3(character->scene->collision_geometry, &character->position, &query)) {
        return 0;
    }

    fw64CollisionGeometryCell* cell = query.cells[0];
    fw64CollisionLadder* ladders = character->scene->collision_geometry->ladders + cell->ladder_index;

    float closest_distance = FLT_MAX;
    fw64CollisionLadder* closest_ladder = NULL;
    Vec3 closest_point;

    for (uint32_t i = 0; i < cell->ladder_count; i++) {
        fw64CollisionLadder* ladder = ladders + i;
        Vec3 current_point;
        fw64_closest_point_on_line_segment(&ladder->entrance, &ladder->exit, &character->position, &current_point);
        float distance = vec3_distance_squared(&character->position, &current_point);
        if (distance <= ladder->radius * ladder->radius && distance < closest_distance) {
            closest_distance = distance;
            closest_ladder = ladder;
            closest_point = current_point;
        }
    }

    if (!closest_ladder) {
        return 0;
    }

    // if we are entering the ladder from the ground, determine if it is the bottom or top
    // if we are entering from the top we need to play the enter ladder from top animation
    fw64CharacterState initial_ladder_state = FW64_CHARACTER_STATE_CLIMB_LADDER_IDLE;
    if (fw64_character_is_on_ground(character)) {
        const float dist_to_exit = vec3_distance_squared(&character->position, &closest_ladder->exit);
        const float dist_to_entrance = vec3_distance_squared(&character->position, &closest_ladder->entrance);

        if (dist_to_exit < dist_to_entrance) {
            initial_ladder_state= FW64_CHARACTER_STATE_LADDER_ENTER_TOP;
        }
    }

    character->state = initial_ladder_state;
    character->active_ladder = closest_ladder;
    fw64_character_rotate_to_face_normal(character, &closest_ladder->normal);
    vec3_add_and_scale(&closest_point, &closest_ladder->normal, query_radius, &character->position);

    return 1;
}

void fw64_character_drop_from_ladder(fw64Character* character) {
    if (!character->active_ladder || character->state == FW64_CHARACTER_STATE_CLIMB_LADDER_EXIT) {
        return;
    }

    Vec3 target_pos = character->active_ladder->normal;
    // TODO: determine a better push back amount;
    vec3_scale(&target_pos, character->active_ladder->radius * 2.0f, &target_pos);
    vec3_add(&character->position, &target_pos, &target_pos);

    fw64_character_set_position(character, &target_pos);
}

static void fw64_character_fixed_update_ladder(fw64Character* character, float time_delta) {
    if (fw64_character_is_entering_ladder(character) || fw64_character_is_exiting_ladder(character)) {
        return;
    }

    if (character->attempt_to_move.y > 0.0f) {
        // is the character trying to climb off the top of the ladder?
        if (character->position.y + character->head_height >= character->active_ladder->exit.y + character->ladder_exit_height_adjustment) {
            character->state = FW64_CHARACTER_STATE_CLIMB_LADDER_EXIT;
            return;
        }

        character->state = FW64_CHARACTER_STATE_CLIMB_LADDER_UP;
    } else if (character->attempt_to_move.y < 0.0f) {
        character->state = FW64_CHARACTER_STATE_CLIMB_LADDER_DOWN;
    } else {
        character->state = FW64_CHARACTER_STATE_CLIMB_LADDER_IDLE;
    }

    character->position.y += character->attempt_to_move.y * character->ladder_climb_speed * time_delta;

    // check if the player climbed to the bottom of the ladder.
    if (character->previous_position.y >= character->active_ladder->entrance.y && character->position.y <= character->active_ladder->entrance.y) {
        fw64_character_drop_from_ladder(character);
    }
}

void fw64_character_fixed_update(fw64Character* character, float time_delta) {
    character->previous_state = character->state;
    character->previous_position = character->position;

    if (!fw64_character_is_enabled(character) || fw64_character_is_interacting_with_ledge(character)) {
        return;
    }

    if (fw64_character_is_on_ladder(character)) {
        fw64_character_fixed_update_ladder(character, time_delta);
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
    float query_radius = character->sphere_query_radius;

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

        fw64_character_check_floor_collision(character, i, &query_pos, query_radius, &query);

        vec3_add_and_scale(&character->position, &up, query_radius, &query_pos);
        fw64_character_check_wall_collision(character, i, &query_pos, query_radius, &query);
    }

    fw64_character_update_capsule(character);

    if (fw64_character_attempt_ladder_grab(character, query_radius)) {
        return;
    }

    // if we are on the ground and not jumping, we would like to attempt to stick to the ground
    // if we are close to a ground triangle.  This should help prevent the case where we are running slightly
    // faster than gravity can pull us down
    if (character->velocity.y <= 0.0f) {
        fw64CollisionGeometryQuery query;
        fw64_collision_geometry_query_vec3(character->scene->collision_geometry, &character->position, &query);
        _fw64_character_attempt_sticky_ground(character, &character->position, query_radius, &query);
    } else {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
    }

    if (fw64_character_is_on_ground(character)) {
        character->velocity.y = 0.0f;
    } else {
        fw64_character_attempt_ledge_grab(character, query_radius);
    }
}
