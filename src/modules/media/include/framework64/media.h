#pragma once

/** \file media.h */

#include "framework64/data_io.h"
#include "framework64/engine.h"

#include <stdint.h>

#define FW64_MEDIA_MODULE_ID 2

typedef struct fw64Media fw64Media;
typedef struct fw64MediaDirItr fw64MediaDirItr;

typedef enum {
    FW64_MEDIA_ITEM_NONE,
    FW64_MEDIA_ITEM_FILE,
    FW64_MEDIA_ITEM_DIRECTORY
} fw64MediaItemType;

#ifdef __cplusplus
extern "C" {
#endif

/** This method should be called once during game initialization. */
fw64Media* fw64_media_init(fw64Engine* engine);

/** 
 * Indicates whether removable media is present.
 * For N64: indicates that a SD card has been detected
 * For Desktop: a filesystem directory has been specified.
 * */
int fw64_media_is_present(fw64Media* media);

/**
 * Opens a directory for iteration.
 * A null return value indicates an invalid path or the max number of open directories has been reached.
 * To iterate over items in the directory, use \ref fw64_media_dir_itr_next
*/
fw64MediaDirItr* fw64_media_open_dir(fw64Media* media, const char* path);

/** Closes an open directory */
void fw64_media_close_dir(fw64Media* media, fw64MediaDirItr* itr);

/** 
 * Advances the iterator to the next directory item.
 * If this function returns 0, all items in the directory have been visitied.
 * */
int fw64_media_dir_itr_next(fw64MediaDirItr* itr);

/** 
 * Gets the path of the current directory item.
 * The memory returned from this function is managed by the iterator and does not need to be freed by the user.
 * The data will need to be copied if you need to persist the value of this pointer beyond the next call to \ref fw64_media_dir_itr_next 
 * */
const char* fw64_media_dir_itr_name(fw64MediaDirItr* itr);

/** Gets the type of the item currently pointed to by the directory iterator*/
fw64MediaItemType fw64_media_dir_itr_type(fw64MediaDirItr* itr);

fw64MediaItemType fw64_media_get_path_type(fw64Media* media, const char* path);

/**
 * Opens a datasource for the given path.
 * If this function returns NULL the path was invalid or the maximum number of open data source handles has been reached.
*/
fw64DataSource* fw64_media_open_data_source(fw64Media* media, const char* path);

/** Closes a data source opened with \ref fw64_media_open_data_source */
void fw64_media_close_data_source(fw64Media* media, fw64DataSource* data_source);

fw64DataWriter* fw64_media_open_data_writer(fw64Media* media, const char* path);
void fw64_media_close_data_writer(fw64Media* media, fw64DataWriter* data_writer);

/** Creates a new directory at the given path.
 *  All subdirectories in the path must exist
*/
int fw64_media_create_directory(fw64Media* media, const char* path);

/** Deletes a file or directory path.
 *  Note: if path is a directory, it must be empty.
*/
int fw64_media_remove(fw64Media* media, const char* path);

#ifdef __cplusplus
}
#endif
