#pragma once

#include "framework64/engine.h"

#define MESSAGE_LIST_COUNT 5
#define MESSAGE_LIST_MESSAGE_SIZE 32

typedef struct MessageData MessageData;

typedef struct {
    MessageData* head;
    MessageData* tail;
    size_t size;
} MessageList;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Font* font;

    uint32_t total_message_count;
    uint32_t total_message_data;

    char** message_list;

} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
