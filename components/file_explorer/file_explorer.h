#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"

#define Fw64_FILE_EXPLORER_MAX_DEPTH 8
#define Fw64_FILE_EXPLORER_MAX_PATH_LENGTH 256

typedef struct {
    char path[Fw64_FILE_EXPLORER_MAX_PATH_LENGTH];
    int path_len;
    int component_lengths[Fw64_FILE_EXPLORER_MAX_DEPTH];
    int current_index;
} fw64FileExplorerDirStack;

typedef struct {
    char* name;
    fw64MediaItemType type;
} fw64FileExplorerItem;

typedef struct {
    fw64FileExplorerItem* items;
    int count;
} fw64FileExplorerItemPage;

typedef struct {
    fw64FileExplorerItemPage** pages;
    int page_count;
    int page_capacity;
    int current_page_index;
    int current_page_selected_item;
} fw64FileExplorerDirData;

typedef void (*fw64FileExplorerFilePickedFunc)(const char* path, void* arg);

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64MediaDirItr* dir_itr;
    fw64FileExplorerDirStack dir_stack;
    fw64FileExplorerDirData dir_data;
    int max_page_item_size;
    fw64BumpAllocator dir_allocator;
    fw64FileExplorerFilePickedFunc file_callback;
    void* file_callback_arg;
} fw64FileExplorer;

void fw64_file_explorer_init(fw64FileExplorer* explorer, fw64Engine* engine, fw64Font* font, int max_page_item_size);
void fw64_file_explorer_uninit(fw64FileExplorer* explorer);
void fw64_file_explorer_update(fw64FileExplorer* explorer);
void fw64_file_explorer_draw(fw64FileExplorer* explorer);
void fw64_file_explorer_set_file_picked_callback(fw64FileExplorer* explorer, fw64FileExplorerFilePickedFunc func, void* arg);
