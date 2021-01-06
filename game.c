#include "game.h"

#include <nusys.h>
#include <malloc.h>

#include "assets.h"
Dynamic test;


Game* game_create() {
    Game* game = malloc(64);
    game->camera = malloc(sizeof(Camera));
    game->renderer = renderer_create();


      guOrtho(&game->camera->projection,
	  -(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
	  -(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
	  1.0F, 10.0F, 1.0F);

    return game;
}

void game_update(Game* game, float time_delta) {
    guRotate(&test.modeling, 0.0f, 0.0f, 0.0f, 1.0f);
    guTranslate(&test.translate, 25.0f, 25.0f, 0.0f);
}

void game_draw(Game* game) {
    renderer_begin(game->renderer, game->camera);
    renderer_draw_static(game->renderer, &test, quad_display_list);
    renderer_end(game->renderer);
}