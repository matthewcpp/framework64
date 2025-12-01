#pragma once

#include "framework64/scene.h"
#include "framework64/capsule.h"

/** gravity constant in meters per second */
#define Fw64_CHARACTER_ENV_DEFAULT_GRAVITY -9.81f
#define FW64_CHARACTER_ENV_MAX_FALL_SPEED -15.0f
#define FW64_CHARACTER_ENV_MAX_SUBSTEPS 5
#define FW64_CHARACTER_ENV_HORIZ_MOVE_THRESHOLD 0.01f

#define FW64_CHARACTER_DEFAULT_JUMP_SPEED 6.4f
#define FW64_CHARACTER_DEFAULT_MAX_SPEED 10.0f
#define FW64_CHARACTER_DEFAULT_LADDER_CLIMB_SPEED 4.5f
#define FW64_CHARACTER_DEFAULT_LADDER_HEIGHT_ADJUSTMENT 0.25f
#define FW64_CHARACTER_DEFAULT_GRAVITY_SCALE 1.5f
#define FW64_CHARACTER_DEFAULT_JUMP_FALL_GRAVITY_SCALE 2.0f

#define FW64_CHARACTER_DEFAULT_GROUND_ACCEL 15.0f
#define FW64_CHARACTER_DEFAULT_GROUND_DECEL 30.0f

#ifdef FW64_COLLISION_GEOMETRY_DEBUG_INFO
typedef struct {
    uint32_t sphere_triangles_considered;
    uint32_t sphere_triangles_skipped;
    uint32_t sphere_triangles_checked;
    uint32_t ray_triangles_considered;
    uint32_t ray_triangles_skipped;
    uint32_t ray_triangles_checked;
} fw64CharacterEnvironmentDebugInfo;

void _fw64_character_environment_debug_info_reset(fw64CharacterEnvironmentDebugInfo* debug);
#define fw64_character_environment_debug_info_reset(debug) _fw64_character_environment_debug_info_reset((debug))
#define fw64_character_environment_increment_ray_triangles_checked(debug, count) (debug)->ray_triangles_checked += (count)
#define fw64_character_environment_increment_sphere_triangles_considered(debug, count) (debug)->sphere_triangles_considered += (count)
#define fw64_character_environment_increment_sphere_triangles_skipped(debug, count) (debug)->sphere_triangles_skipped += (count)
#define fw64_character_environment_increment_sphere_triangles_checked(debug, count) (debug)->sphere_triangles_checked += (count)
#else 
#define fw64_character_environment_debug_info_reset(debug) 
#define fw64_character_environment_increment_ray_triangles_checked(debug, count)
#define fw64_character_environment_increment_sphere_triangles_considered(debug, count) 
#define fw64_character_environment_increment_sphere_triangles_skipped(debug, count) 
#define fw64_character_environment_increment_sphere_triangles_checked(debug, count) 
#endif


typedef struct {
    Vec3 gravity;
    int max_substeps;

    /** The maximum rate that characters can fall in meters / sec */
    float max_fall_speed;

    /** When grounded, the minimum amount of distance a character must attempt to move horizontally.
      * This is intended to prevent jittering from floating point rounding 
    */
    float horizontal_move_threshold;

#ifdef FW64_COLLISION_GEOMETRY_DEBUG_INFO
    fw64CharacterEnvironmentDebugInfo debug_info;
#endif
} fw64CharacterEnvironment;

void fw64_character_envionment_init(fw64CharacterEnvironment* env);

typedef enum {
    FW64_CHARACTER_STATE_DISABLED,
    FW64_CHARACTER_STATE_ON_GROUND,
    FW64_CHARACTER_STATE_IN_AIR,
    FW64_CHARACTER_STATE_LEDGE_HANG,
    FW64_CHARACTER_STATE_LEDGE_CLIMB_UP,
    FW64_CHARACTER_STATE_LADDER_ENTER_TOP,
    FW64_CHARACTER_STATE_CLIMB_LADDER_IDLE,
    FW64_CHARACTER_STATE_CLIMB_LADDER_DOWN,
    FW64_CHARACTER_STATE_CLIMB_LADDER_UP,
    FW64_CHARACTER_STATE_CLIMB_LADDER_EXIT
} fw64CharacterState;

typedef struct {
    fw64CharacterEnvironment* environment;
    fw64Node* node;
    fw64Scene* scene;

    Vec3 velocity;
    fw64CharacterState previous_state, state;
    /** This is interpreted as the bottom center of the character. */
    Vec3 previous_position, position;

    /* The size of the character in each dimension*/
    Vec3 size;

    float step_height;

    /** This the height at which forward raycasting will orginate at when attempting ledge grabbing
     * This value should be set as the height of the characters head */
    float head_height;

    /** an amount to offset the character's position by when hanging from ledges */
    float hang_vertical_offset;

    /** The maximum distance a character can move in one second */
    float max_speed;

    float jump_speed;
    float gravity_scale;
    float jump_fall_gravity_scale;

    float ground_accel;
    float ground_decel;

    Vec3 attempt_to_move;
    int attempt_to_jump;

    /** The speed at which the character can climb ladders*/
    float ladder_climb_speed;

    /** A value to add to the calculated ladder exit height.
     * Current use case is to make exit animation line up with geometry if necessary.
     */
    float ladder_exit_height_adjustment;
    fw64CollisionLadder* active_ladder;

} fw64Character;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Node* node, fw64Scene* scene);
void fw64_character_fixed_update(fw64Character* character, float time_delta);

void fw64_character_set_position(fw64Character* character, const Vec3* position);

/** Immediately drops the player from the ledge and sets them in an 'in air' state */
void fw64_character_drop_from_ledge(fw64Character* character);

/** Immediately drops the player from the ladder and sets them in an 'in air' state */
void fw64_character_drop_from_ladder(fw64Character* character);

/** Puts the character in the ledge climbing state.
 *  Note that while in this state the character's position will not be updated and no collision checks will take place.
 * */
void fw64_character_start_climbing_up_ledge(fw64Character* character);

/** Signals that the character his finished climbing up the ledge.
 *  User should supply the new position for the character.
 *  This could be computed by using the foot joint after the climb up animation has completed.
 */
void fw64_character_finish_climbing_up_ledge(fw64Character* character, const Vec3* new_pos);

void fw64_character_finish_exiting_ladder(fw64Character* character, const Vec3* new_pos);
void fw64_character_finish_entering_ladder(fw64Character* character, const Vec3* new_pos);

/** This function is used for debug purposes */
void fw64_character_get_ledge_check_origin(fw64Character* character, Vec3* out);

#define fw64_character_is_on_ground(character) ((character)->state == FW64_CHARACTER_STATE_ON_GROUND)
#define fw64_character_is_in_air(character) ((character)->state == FW64_CHARACTER_STATE_IN_AIR)
#define fw64_character_is_hanging_on_ledge(character) ((character)->state == FW64_CHARACTER_STATE_LEDGE_HANG)
#define fw64_character_is_climbing_up_ledge(character) ((character)->state == FW64_CHARACTER_STATE_LEDGE_CLIMB_UP)

#define fw64_character_is_on_ladder(character) ((character)->active_ladder != NULL)
#define fw64_character_is_entering_ladder(character) ((character)->state == FW64_CHARACTER_STATE_LADDER_ENTER_TOP)
#define fw64_character_is_exiting_ladder(character) ((character)->state == FW64_CHARACTER_STATE_CLIMB_LADDER_EXIT)

/** Returns nonzero value if the character is interacting with the ledge in any way (climbing or hanging) */
#define fw64_character_is_interacting_with_ledge(character) (fw64_character_is_hanging_on_ledge((character)) || fw64_character_is_climbing_up_ledge((character)))
#define fw64_character_is_falling(character) (fw64_character_is_in_air((character)) && (character)->velocity.y < 0.0)

#define fw64_character_is_attempting_to_move(character) ((character)->attempt_to_move.x != 0.0f || (character)->attempt_to_move.z != 0.0f)
#define fw64_character_is_moving_horizontally(character) ((character)->velocity.x != 0.0f || (character)->velocity.z != 0.0f)

#define fw64_character_disable(character) ((character)->state = FW64_CHARACTER_STATE_DISABLED)
#define fw64_character_enable(character) ((character)->state = FW64_CHARACTER_STATE_IN_AIR)
#define fw64_character_is_enabled(character) ((character)->state != FW64_CHARACTER_STATE_DISABLED)

#ifdef __cplusplus
}
#endif
