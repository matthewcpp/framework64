#include "fw64_character.h"

#include <framework64/log.h>
#include <framework64/math.h>
#include <framework64/types.h>

#include <string.h>

void fw64_character_envionment_init(fw64CharacterEnvironment* env) {
    vec3_set(&env->gravity, 0.0f, Fw64_CHARACTER_ENV_DEFAULT_GRAVITY, 0.0f);
    env->max_fall_speed = FW64_CHARACTER_ENV_MAX_FALL_SPEED;
    env->max_substeps = FW64_CHARACTER_ENV_MAX_SUBSTEPS;
    env->horizontal_move_threshold_sq = FW64_CHARACTER_ENV_HORIZ_MOVE_THRESHOLD * FW64_CHARACTER_ENV_HORIZ_MOVE_THRESHOLD;
}

#ifdef FW64_COLLISION_GEOMETRY_DEBUG_INFO
void _fw64_character_environment_debug_info_reset(fw64CharacterEnvironmentDebugInfo* debug) {
    memset(debug, 0, sizeof(fw64CharacterEnvironmentDebugInfo));
}
#endif

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Node* node, fw64Scene* scene) {
    character->environment = env;
    character->node = node;
    character->scene = scene;

    Vec3 zero = vec3_zero(), one = vec3_one();
    character->attempt_to_move = zero;
    fw64_character_set_position(character, &zero);
    fw64_character_set_size(character, &one);
    
    character->step_height = FW64_CHARACTER_DEFAULT_STEP_HEIGHT;
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

static fw64CollisionTriangle* _get_bounding_volume_floors(const fw64CollisionGeometry* collision_geometry, const fw64CollisionGeometryBoundingVolume* bounding_volume, uint16_t* count) {
    *count = bounding_volume->floor_count;
    return collision_geometry->triangles + bounding_volume->floor_index;
}

static int _resolve_triangle_collision(fw64Character* character, fw64CollisionTriangle* triangle, const Vec3* capsule_point, const Vec3* hit_point, void* arg) 
{
    (void)arg;
    // The distance between the hit point on the triangle and the capsule point
    float distance = vec3_distance(hit_point, capsule_point);

    // 1. Calculate the true push-out normal
    Vec3 push_out_normal;
    if (distance > 0.0001f) {
        // Vector from the hit point TO the capsule
        vec3_subtract(capsule_point, hit_point, &push_out_normal);
        vec3_normalize(&push_out_normal);
    } else {
        push_out_normal = triangle->N; 
    }

    // 2. Correct position along the calculated normal
    Vec3 correction_vector;
    float penetration = character->capsule.radius - distance;
    
    float slop = 0.001f; // A tiny allowed penetration distance
    if (penetration > slop) {
        vec3_scale(&push_out_normal, penetration, &correction_vector);
        vec3_add(&character->position, &correction_vector, &character->position);
    }

    // 3. Velocity Deflection (Sliding)
    float dot_product = vec3_dot(&character->velocity, &push_out_normal);
    
    if (dot_product < 0.0f) {
        float original_speed = vec3_length(&character->velocity);

        Vec3 rejection;
        vec3_scale(&push_out_normal, dot_product, &rejection);
        vec3_subtract(&character->velocity, &rejection, &character->velocity);

        // Preserve speed while running up slopes by normalizing and scaling the projected
        // vector to have the same length as our original speed
        if (original_speed > 0.0001f) {
            vec3_normalize(&character->velocity);
            vec3_scale(&character->velocity, original_speed, &character->velocity);
        }
    }

    fw64_character_update_capsule(character);

     return 1;
}

static int _resolve_wall_collision(fw64Character* character, fw64CollisionTriangle* triangle, const Vec3* capsule_point, const Vec3* hit_point, void* arg) {
(void)arg;

    // TODO: Staircase / Step-up logic
    // if (hit_point->y - character->position.y <= character->step_height) {
        // fw64_log_message("attempt step up: %f (%f)", (hit_point->y - character->position.y), character->step_height);
    // }

    // --- STEP A: Push-Out (Position Correction) ---
    float distance = vec3_distance(hit_point, capsule_point);
    Vec3 push_out_normal;

    if (distance > 0.0001f) {
        vec3_subtract(capsule_point, hit_point, &push_out_normal);
        vec3_normalize(&push_out_normal);
    } else {
        // Deep piercing fallback
        push_out_normal = triangle->N; 
    }

    float penetration = character->capsule.radius - distance;
    float slop = 0.001f; // Standard allowed penetration depth
    
    // Correct position along the calculated normal
    if (penetration > slop) {
        Vec3 correction_vector;
        vec3_scale(&push_out_normal, penetration, &correction_vector);
        vec3_add(&character->position, &correction_vector, &character->position);
    }


    // --- STEP B: Horizontal Velocity Deflection (Wall Sliding) ---
    // We calculate the dot product using ONLY X and Z.
    // This perfectly isolates horizontal movement from gravity and jumps.
    float dot_product = (character->velocity.x * push_out_normal.x) + 
                        (character->velocity.z * push_out_normal.z);
    
    // If dot_product is negative, the character is moving INTO the wall
    if (dot_product < 0.0f) {
        // Subtract the rejection vector from the character's horizontal velocity.
        // This strips away inward momentum, leaving only the sliding momentum.
        character->velocity.x -= push_out_normal.x * dot_product;
        character->velocity.z -= push_out_normal.z * dot_product;
    }

    // Update the capsule bounds for the next potential collision check in the substep loop
    fw64_character_update_capsule(character);

    return 1;
}

static fw64CollisionTriangle* _get_bounding_volume_walls(const fw64CollisionGeometry* collision_geometry, const fw64CollisionGeometryBoundingVolume* bounding_volume, uint16_t* count) {
    *count = bounding_volume->wall_count;
    return collision_geometry->triangles + bounding_volume->wall_index;
}

typedef fw64CollisionTriangle* (*_GetBoundingVolumeTriangleFunc)(const fw64CollisionGeometry* collision_geometry, const fw64CollisionGeometryBoundingVolume* bounding_volume, uint16_t* count);
typedef int (*_ResolveCapsuleTriangleCollisionFunc)(fw64Character* character, fw64CollisionTriangle* triangle, const Vec3* capsule_point, const Vec3* hit_point, void* arg);

// WIP
static int fw64_character_check_capsule_collision(fw64Character* character, const fw64Capsule* capsule, fw64CollisionGeometryQuery* query, _GetBoundingVolumeTriangleFunc get_bounding_volume_triangles, _ResolveCapsuleTriangleCollisionFunc resolve_func, void* resolve_func_arg) {
    Vec3 hit_point, capsule_pt;
    uint16_t triangle_count;
    int hit_count = 0;

    for (uint32_t c = 0; c < query->cell_count; c++) {
        fw64CollisionGeometryCell* cell = query->cells[c];

        for (uint16_t i = 0; i < cell->bounding_volume_count; i++) {
            const fw64CollisionGeometryBoundingVolume* bounding_volume = character->scene->collision_geometry->bounding_volumes + cell->bounding_volume_index + i;
            fw64CollisionTriangle* triangles = get_bounding_volume_triangles(character->scene->collision_geometry, bounding_volume, &triangle_count);
            fw64_character_environment_increment_capsule_triangles_considered(&character->environment->debug_info, triangle_count);

            if (!box_intersection(&bounding_volume->primitive, &capsule->aabb)) {
                fw64_character_environment_increment_capsule_triangles_skipped(&character->environment->debug_info, triangle_count);
                continue;
            }

            for (uint32_t t = 0; t < triangle_count; t++) {
                fw64CollisionTriangle* triangle = triangles + t;

                // filter triangles whose aabb does not intersect our capsule
                if (!box_intersection(&capsule->aabb, &triangle->bounding)) {
                    fw64_character_environment_increment_capsule_triangles_skipped(&character->environment->debug_info, 1);
                    continue;
                }

                // precision check
                fw64_character_environment_increment_capsule_triangles_checked(&character->environment->debug_info, 1);
                if (fw64_collision_test_capsule_triangle(capsule, &triangle->A, &triangle->B, &triangle->C, &triangle->N, &hit_point, &capsule_pt)) {
                    resolve_func(character, triangle, &capsule_pt, &hit_point, resolve_func_arg);
                    hit_count += 1;
                }
            }
        }
    }

    return hit_count;
}

// TODO: https://brendankeesing.com/blog/character_controller_stairs/
static void fw64_character_check_floor_collision(fw64Character* character, fw64CollisionGeometryQuery* query) {
    int hit_count = fw64_character_check_capsule_collision(character, &character->capsule, query, _get_bounding_volume_floors, _resolve_triangle_collision, NULL);

    // resolve all collisions
    if (hit_count > 0) {
        // attempt to prevent jittering by skipping slight movement that may arise due to floating point effects
        if (fw64_character_is_on_ground(character) && !fw64_character_is_attempting_to_move(character) &&
            vec3_distance_squared(&character->previous_position, &character->position) < character->environment->horizontal_move_threshold_sq) {
            character->position = character->previous_position;
            vec3_set_zero(&character->velocity);
            fw64_character_update_capsule(character);
        }

        character->state = FW64_CHARACTER_STATE_ON_GROUND;
    } else {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
    }
}

typedef struct {
    Vec3 penetration_normal;
    float min_vertical_drop;
    int has_floor;
} _fw64CharacterStickyGroundArg;

static int _resolve_sticky_ground_collision(fw64Character* character, fw64CollisionTriangle* triangle, const Vec3* capsule_point, const Vec3* hit_point, void* arg) {
    _fw64CharacterStickyGroundArg* sticky_ground = (_fw64CharacterStickyGroundArg*)arg;

    float distance = vec3_distance(hit_point, capsule_point);
    Vec3 push_out_normal;

    if (distance > 0.0001f) {
        vec3_subtract(capsule_point, hit_point, &push_out_normal);
        vec3_normalize(&push_out_normal);
    } else {
        push_out_normal = triangle->N;
    }

    // Is the probe actually penetrating the geometry?
    float probe_penetration = character->capsule.radius - distance;

    // FILTER: We only care if the probe actually hit it
    if (probe_penetration > 0.0f) {
        
        // Calculate the distance from the CHARACTER'S current feet to the contact plane
        Vec3 diff;
        vec3_subtract(&character->capsule.a, hit_point, &diff);
        float current_dist_to_plane = vec3_dot(&diff, &push_out_normal);
        
        // Calculate the exact vertical drop required to make the character perfectly flush
        // Dividing by push_out_normal.y perfectly accounts for the slant of slopes!
        float vertical_drop = (current_dist_to_plane - character->capsule.radius) / push_out_normal.y;
        
        // Clamp to 0 just in case floating point noise makes it slightly negative
        if (vertical_drop < 0.0f){
            vertical_drop = 0.0f;
        }

        // We want to snap to the HIGHEST floor (which means the smallest required drop)
        if (!sticky_ground->has_floor || vertical_drop < sticky_ground->min_vertical_drop) {
            sticky_ground->min_vertical_drop = vertical_drop;
            sticky_ground->penetration_normal = push_out_normal;
            sticky_ground->has_floor = 1;
        }
    }

    return 0;
}

static void _fw64_character_attempt_sticky_ground(fw64Character* character, float time_delta) {
    _fw64CharacterStickyGroundArg sticky_ground_arg;
    sticky_ground_arg.has_floor = 0;

    float horizontal_vel_sq = (character->velocity.x * character->velocity.x) + 
                            (character->velocity.z * character->velocity.z);

    // We need the actual length for the threshold
    float horizontal_speed = 0.0f;
    if (horizontal_vel_sq > 0.0f) {
        horizontal_speed = fw64_sqrtf(horizontal_vel_sq); 
    }

    // Convert speed to per-frame distance
    float horizontal_movement = horizontal_speed * time_delta;

    // Max Step Height + How far the ground could drop this frame + a tiny buffer
    float sticky_threshold = character->step_height + horizontal_movement + 0.05f;

    
    fw64Capsule probe;
    probe.radius = character->capsule.radius;
    const Vec3 r_vec = {probe.radius, probe.radius, probe.radius};

    // the top of the probe is set to the feet of the  character
    probe.b = character->capsule.a;
    vec3_add(&probe.b, &r_vec, &probe.aabb.max);

    // the bottom of the probe is offset by the sticky ground depth
    probe.a = character->capsule.a;
    probe.a.y -= sticky_threshold;
    vec3_subtract(&probe.a, &r_vec, &probe.aabb.min);
    
    fw64CollisionGeometryQuery query;
    fw64_collision_geometry_query_vec3(character->scene->collision_geometry, &probe.a, &query);

    fw64_character_check_capsule_collision(character, &probe, &query, _get_bounding_volume_floors, _resolve_sticky_ground_collision, &sticky_ground_arg);

    // If we found a valid floor, AND the required drop is within our allowed threshold
    if (sticky_ground_arg.has_floor && sticky_ground_arg.min_vertical_drop <= sticky_threshold) {
        character->position.y -= sticky_ground_arg.min_vertical_drop;
        
        character->state = FW64_CHARACTER_STATE_ON_GROUND;
        character->velocity.y = 0.0f;
        fw64_character_update_capsule(character);
    } else {
        character->state = FW64_CHARACTER_STATE_IN_AIR;
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
    vec3_scale(&back, character->capsule.radius, &back);
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

static void fw64_character_check_wall_collision(fw64Character* character, fw64CollisionGeometryQuery* query) {
    fw64_character_check_capsule_collision(character, &character->capsule, query, _get_bounding_volume_walls, _resolve_wall_collision, NULL);
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
    int character_did_jump = 0;
    if (character->attempt_to_jump && fw64_character_is_on_ground(character)) {
        character->velocity.y += character->jump_speed;
        character->state = FW64_CHARACTER_STATE_IN_AIR;
        character_did_jump = 1;
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
    float query_radius = character->capsule.radius;

    // determine how many substeps we will need to avoid tunneling
    const float max_step = query_radius * 0.5f;
    int num_substeps = fw64_clampi((int)fw64_ceilf(move_distance / max_step), 1, character->environment->max_substeps);

    // update character's position over each substep
    float substep_dt = time_delta / (float)num_substeps;
    for (int i = 0; i < num_substeps; i++) {
        Vec3 step_move = character->velocity, query_pos, up = vec3_up();
        vec3_scale(&step_move, substep_dt, &step_move);
        vec3_add(&character->position, &step_move, &character->position);
        fw64_character_update_capsule(character);

        vec3_add_and_scale(&character->position, &up, query_radius, &query_pos);

        fw64CollisionGeometryQuery query;
        fw64_collision_geometry_query_vec3(character->scene->collision_geometry, &query_pos, &query);

        fw64_character_check_floor_collision(character, &query);

        vec3_add_and_scale(&character->position, &up, query_radius, &query_pos);
        fw64_character_check_wall_collision(character, &query);
    }

    if (fw64_character_attempt_ladder_grab(character, query_radius)) {
        return;
    }

    // if we were on the ground and not jumping, we would like to attempt to stick to the ground
    // if we are close to a ground triangle.  This should help prevent the case where we are running slightly
    // faster than gravity can pull us down
    if (character->previous_state == FW64_CHARACTER_STATE_ON_GROUND && !character_did_jump) {
        _fw64_character_attempt_sticky_ground(character, time_delta);
    }

    if (fw64_character_is_on_ground(character)) {
        character->velocity.y = 0.0f;
    } else {
        fw64_character_attempt_ledge_grab(character, query_radius);
    }

    vec3_set_zero(&character->attempt_to_move);
    character->attempt_to_jump = 0;

}
