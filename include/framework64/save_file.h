#pragma once

/** \file save_file.h */

#include <stdint.h>

typedef struct fw64SaveFile fw64SaveFile;

#ifdef __cplusplus
extern "C" {
#endif

/** returns whether the save file functionality is available */
int fw64_save_file_valid(fw64SaveFile* save_file);

/** The size (in bytes) of the save file */
uint32_t fw64_save_file_size(fw64SaveFile* save_file);

/** 
 * Starts a save file write operation.
 * This method will return immediately, however the write operation is not synchronous and will complete after some time.
 * During this time \ref fw64_save_file_busy will return true
 * It is the caller's responsibility to ensure that the buffer passed into this function remains in a vaild state until the operation completes. 
 * \param base_addr byte offset of the save file to write to.  This value should be a multiple of 8 on N64 platform.
 * \returns zero if the operation started successfully.  A non zero value is returned if there is no save file capability or another write operation was active when this method was called.
 */
int fw64_save_file_write(fw64SaveFile* save_file, uint32_t base_addr, void* buffer, uint32_t buffer_size);


/**
 * Reads data from the save file and fills the supplied buffer.
 * \param base_addr byte offset of the save file to write to.  This value should be a multiple of 8 on N64 platform.
 * \returns zero if the operation started successfully.  A non zero value is returned if there is no save file capability
 */
int fw64_save_file_read(fw64SaveFile* save_file, uint32_t base_addr, void* buffer, uint32_t buffer_size);

/** Returns a non zero value if the save file is currently performing a read / write action.*/
int fw64_save_file_busy(fw64SaveFile* save_file);

#ifdef __cplusplus
}
#endif