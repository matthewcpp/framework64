#include "framework64/n64/save_file.h"

#include <string.h>

#define SAVE_FILE_THREAD_STACK_SIZE 512
#define SAVE_FILE_THREAD_ID    20
#define SAVE_FILE_THREAD_PRI   1

static OSThread save_file_thread;
static u8 save_file_thread_stack[SAVE_FILE_THREAD_STACK_SIZE];

static void save_file_thread_main(void* arg) {
    fw64SaveFile* save_file = (fw64SaveFile*)arg;

    for (;;) {
        if (save_file->write_buffer != NULL) {
            u8 block = save_file->write_base_addr / 8;
            nuEepromWrite(block, save_file->write_buffer, save_file->write_size);
            save_file->write_buffer = NULL;
        }

        osYieldThread();
    }
}

void fw64_n64_save_file_init(fw64SaveFile* save_file) {
    memset(save_file, 0, sizeof(fw64SaveFile));

    save_file->eeprom_type = nuEepromCheck();

    if (fw64_save_file_valid(save_file)) {
        osCreateThread( &save_file_thread, SAVE_FILE_THREAD_ID, save_file_thread_main, save_file, 
                        (save_file_thread_stack + SAVE_FILE_THREAD_STACK_SIZE), 
                        SAVE_FILE_THREAD_PRI);
        osStartThread(&save_file_thread);
    }
}

int fw64_save_file_valid(fw64SaveFile* save_file) {
    return save_file->eeprom_type > 0;
}

uint32_t fw64_save_file_size(fw64SaveFile* save_file) {
    switch (save_file->eeprom_type) {
        case EEPROM_TYPE_4K:
            return EEPROM_MAXBLOCKS * EEPROM_BLOCK_SIZE;

        case EEPROM_TYPE_16K:
            return EEP16K_MAXBLOCKS * EEPROM_BLOCK_SIZE;
        
        default:
            return 0;
    }
}

int fw64_save_file_write(fw64SaveFile* save_file, uint32_t base_addr, void* buffer, uint32_t buffer_size) {
    if (!fw64_save_file_valid(save_file) || fw64_save_file_busy(save_file))
        return 1;

    save_file->write_buffer = (u8*)buffer;
    save_file->write_base_addr = base_addr;
    save_file->write_size = buffer_size;

    return 0;
}

int fw64_save_file_read(fw64SaveFile* save_file, uint32_t base_addr, void* buffer, uint32_t buffer_size) {
    if (!fw64_save_file_valid(save_file) || fw64_save_file_busy(save_file))
        return 1;

    (void)save_file;
    u8 block = base_addr / 8;

    return nuEepromRead(block, (u8*)buffer, buffer_size);
}

int fw64_save_file_busy(fw64SaveFile* save_file) {
    return save_file->write_buffer != NULL;
}