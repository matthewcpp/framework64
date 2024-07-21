#include "fw64_media_file_picker.h"

#include "framework64/controller_mapping/n64.h"

#include <string.h>
#include <stdlib.h>

#define DIR_ALLOCATOR_SIZE 8 * 1024
#define DEFAULT_PAGE_CAPCITY 8

static void push_directory(fw64MediaFilePicker* explorer, const char* path);
static int dir_stack_append(fw64FileExplorerDirStack* dir_stack, const char* path);
static void reset_dir_data(fw64MediaFilePicker* explorer);
static void read_next_page(fw64MediaFilePicker* explorer);
static void read_path(fw64MediaFilePicker* explorer);


void fw64_media_file_picker_init(fw64MediaFilePicker* explorer, fw64Engine* engine, int max_page_item_size) {
    explorer->engine = engine;
    explorer->max_page_item_size = max_page_item_size;
    explorer->flags = FW64_MEDIA_FILEPICKER_FLAG_NONE;

    explorer->dir_itr = NULL;
    explorer->file_callback = NULL;

    strcpy(explorer->dir_stack.path, "/");
    explorer->dir_stack.path_len = 1;
    explorer->dir_stack.component_lengths[0] = 0;
    explorer->dir_stack.current_index = 0;

    fw64_bump_allocator_init(&explorer->dir_allocator, DIR_ALLOCATOR_SIZE);
    fw64_ui_navigation_init(&explorer->ui_nav, engine->input, 0);
    read_path(explorer);
}

void fw64_media_file_picker_uninit(fw64MediaFilePicker* explorer) {
    fw64_bump_allocator_uninit(&explorer->dir_allocator);
}

void read_path(fw64MediaFilePicker* explorer) {
    if (explorer->dir_itr) {
        fw64_media_close_dir(explorer->engine->media, explorer->dir_itr);
    }

    explorer->dir_itr = fw64_media_open_dir(explorer->engine->media, explorer->dir_stack.path);

    fw64_bump_allocator_reset(&explorer->dir_allocator);
    reset_dir_data(explorer);

    if (explorer->dir_itr)
        read_next_page(explorer);

    explorer->dir_data.current_page_selected_item = 0;
}

void push_directory(fw64MediaFilePicker* explorer, const char* path) {
    int dir_appended = dir_stack_append(&explorer->dir_stack, path);

    if (!dir_appended)
        return;

    read_path(explorer);
}

static void pop_directory(fw64MediaFilePicker* explorer) {
    fw64FileExplorerDirStack* dir_stack = &explorer->dir_stack;

    if (dir_stack->current_index == 0)
        return;

    dir_stack->path_len -= dir_stack->component_lengths[dir_stack->current_index--];
    dir_stack->path[dir_stack->path_len] = 0;

    read_path(explorer);
}

// todo: more error checking here
int dir_stack_append(fw64FileExplorerDirStack* dir_stack, const char* path) {
    if (dir_stack->current_index > 0)
        dir_stack->path[dir_stack->path_len++] = '/';

    size_t path_len = strlen(path);
    strcpy(&dir_stack->path[dir_stack->path_len], path);
    dir_stack->path_len += path_len;
    dir_stack->component_lengths[++dir_stack->current_index] = path_len;

    return 1;
}

void read_next_page(fw64MediaFilePicker* explorer) {
    fw64FileExplorerItemPage* page = fw64_bump_allocator_malloc(&explorer->dir_allocator, sizeof(fw64FileExplorerItemPage));

    page->items = fw64_bump_allocator_malloc(&explorer->dir_allocator, sizeof(fw64FileExplorerItem) * explorer->max_page_item_size);
    page->count = 0;
  
    for (int i = 0; i < explorer->max_page_item_size; i++) {
        if (!fw64_media_dir_itr_next(explorer->dir_itr))
            break;

        fw64FileExplorerItem* item = page->items + page->count;
        
        const char* item_name = fw64_media_dir_itr_name(explorer->dir_itr);
        size_t name_len = strlen(item_name);
        item->name = fw64_bump_allocator_malloc(&explorer->dir_allocator, name_len + 1);
        strcpy(item->name, item_name);
        item->type = fw64_media_dir_itr_type(explorer->dir_itr);

        page->count += 1;
    }

    if (explorer->dir_data.page_count == explorer->dir_data.page_capacity) {
        explorer->dir_data.page_capacity += DEFAULT_PAGE_CAPCITY;
        explorer->dir_data.pages = fw64_bump_allocator_realloc(&explorer->dir_allocator, explorer->dir_data.pages, sizeof(fw64FileExplorerItemPage*) * explorer->dir_data.page_capacity);
    }

    explorer->dir_data.pages[explorer->dir_data.page_count++] = page;
}

static void change_page_selection(fw64MediaFilePicker* explorer, int direction) {
    fw64FileExplorerItemPage* page = explorer->dir_data.pages[explorer->dir_data.current_page_index];

    if (page->count == 0)
        return;

    int new_index = explorer->dir_data.current_page_selected_item + direction;

    if (new_index < 0)
        new_index = page->count - 1;
    else if (new_index >= page->count)
        new_index = 0;

    explorer->dir_data.current_page_selected_item = new_index;
    explorer->flags |= FW64_MEDIA_FILEPICKER_FLAG_SELECTION_CHANGED;
}

static void current_item_activated(fw64MediaFilePicker* explorer) {
    fw64FileExplorerItemPage* page = explorer->dir_data.pages[explorer->dir_data.current_page_index];
    fw64FileExplorerItem* item = page->items + explorer->dir_data.current_page_selected_item;

    if (page->count == 0)
        return;

    if (item->type == FW64_MEDIA_ITEM_DIRECTORY) {
        push_directory(explorer, item->name);
    }
    else if (explorer->file_callback) {
        char file_path[512];
        strcpy(file_path, explorer->dir_stack.path);
        size_t path_dir_len = explorer->dir_stack.path_len;

        if (explorer->dir_stack.path_len > 1) {
            file_path[path_dir_len++] = '/';
        }
            
        strcpy(&file_path[0] + path_dir_len, item->name);

        explorer->flags |= FW64_MEDIA_FILEPCIKER_FLAG_PICKED_FILE;
        explorer->file_callback(file_path, explorer->file_callback_arg);
    }
}

void fw64_media_file_picker_update(fw64MediaFilePicker* explorer) {
    fw64_ui_navigation_update(&explorer->ui_nav, explorer->engine->time->time_delta);
    explorer->flags = FW64_MEDIA_FILEPICKER_FLAG_NONE;

    if (fw64_ui_navigation_moved_down(&explorer->ui_nav)) {
        change_page_selection(explorer, 1);
    } else if (fw64_ui_navigation_moved_up(&explorer->ui_nav)) {
        change_page_selection(explorer, -1);
    } else if (fw64_ui_navigation_accepted(&explorer->ui_nav)) {
        current_item_activated(explorer);
    } else if (fw64_ui_navigation_back(&explorer->ui_nav)) {
        pop_directory(explorer);
    }
}

void reset_dir_data(fw64MediaFilePicker* explorer) {
    fw64FileExplorerDirData* dir_data = &explorer->dir_data;

    dir_data->page_count = 0;
    dir_data->page_capacity = DEFAULT_PAGE_CAPCITY;
    dir_data->pages = fw64_bump_allocator_malloc(&explorer->dir_allocator, sizeof(fw64FileExplorerItemPage*) * explorer->dir_data.page_capacity);
    dir_data->current_page_index = 0;
}

void fw64_media_file_picker_set_file_picked_callback(fw64MediaFilePicker* explorer, fw64MediaFilePickerFilePickedFunc func, void* arg) {
    explorer->file_callback = func;
    explorer->file_callback_arg = arg;
}
