#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"

#include "framework64/media.h"

#include "fw64_ui_navigation.h"

#define fw64_media_file_picker_MAX_DEPTH 8
#define fw64_media_file_picker_MAX_PATH_LENGTH 256

typedef enum {
    FW64_MEDIA_FILEPICKER_FLAG_NONE,
    FW64_MEDIA_FILEPICKER_FLAG_SELECTION_CHANGED = 1 << 0,
    FW64_MEDIA_FILEPCIKER_FLAG_PICKED_FILE       = 1 << 1,
    FW64_MEDIA_FILEPCIKER_FLAG_DIR_CHANGED       = 1 << 2
} fw64MediaFilePickerFlags;

typedef struct {
    char path[fw64_media_file_picker_MAX_PATH_LENGTH];
    int path_len;
    int component_lengths[fw64_media_file_picker_MAX_DEPTH];
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

typedef void (*fw64MediaFilePickerFilePickedFunc)(const char* path, void* arg);

typedef struct {
    fw64Engine* engine;
    fw64Media* media;
    fw64MediaDirItr* dir_itr;
    fw64FileExplorerDirStack dir_stack;
    fw64FileExplorerDirData dir_data;
    int max_page_item_size;
    fw64BumpAllocator dir_allocator;
    fw64UiNavigation ui_nav;
    fw64MediaFilePickerFilePickedFunc file_callback;
    void* file_callback_arg;
    fw64MediaFilePickerFlags flags;
} fw64MediaFilePicker;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_media_file_picker_init(fw64MediaFilePicker* explorer, fw64Engine* engine, int max_page_item_size);
void fw64_media_file_picker_uninit(fw64MediaFilePicker* explorer);
void fw64_media_file_picker_update(fw64MediaFilePicker* explorer);
void fw64_media_file_picker_draw(fw64MediaFilePicker* explorer);
void fw64_media_file_picker_set_file_picked_callback(fw64MediaFilePicker* explorer, fw64MediaFilePickerFilePickedFunc func, void* arg);

#define fw64_media_file_picker_did_change_selection(picker) ((picker)->flags & FW64_MEDIA_FILEPICKER_FLAG_SELECTION_CHANGED)
#define fw64_media_file_picker_did_change_directory(picker) ((picker)->flags & FW64_MEDIA_FILEPCIKER_FLAG_DIR_CHANGED)
#define fw64_media_file_picker_did_change(picker) ((picker)->flags & (FW64_MEDIA_FILEPCIKER_FLAG_DIR_CHANGED | FW64_MEDIA_FILEPICKER_FLAG_SELECTION_CHANGED))
#define fw64_media_file_picker_did_pick_file(picker) ((picker)->flags & FW64_MEDIA_FILEPCIKER_FLAG_PICKED_FILE)

#ifdef __cplusplus
}
#endif
