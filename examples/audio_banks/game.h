#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_rotate_node.h"

typedef struct {
    fw64Scene scene;
    RotateNode rotate_node;
    fw64RenderPass* renderpass;
    fw64Engine* engine;
} SceneView;

void scene_view_init(SceneView* scene_view, fw64Engine* engine);
void scene_view_update(SceneView* scene_view);
void scene_view_draw(SceneView* scene_view);

typedef struct {
    int sound_bank;
    int sound_num;
    int sound_id;

    int music_bank;
    int music_track;
} AudioState;

typedef struct {
    fw64Engine* engine;
    AudioState* audio_state;
} Controller;

void controller_init(Controller* controller, fw64Engine* engine, AudioState* audio_state);
void controller_update(Controller* controller);

typedef struct {
    fw64Engine* engine;
    AudioState* audio_state;
    fw64Texture* buttons;
    fw64Font* font;
    fw64SpriteBatch* sprite_batch;
    fw64RenderPass* renderpass;
} UI;

void ui_init(UI* ui, fw64Engine* engine, AudioState* audio_state);
void ui_draw(UI* ui);

typedef struct {
    fw64Engine* engine;
    AudioState audio_state;
    SceneView scene_view;
    Controller controller;
    UI ui;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game *game, fw64Engine *engine);
void game_update(Game *game);
void game_draw(Game *game);

#ifdef __cplusplus
}
#endif
