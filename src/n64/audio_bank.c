#include "framework64/n64/audio_bank.h"
#include "framework64/n64/filesystem.h"

#include <stddef.h>

typedef struct {
    uint32_t song_count;
    uint32_t ctrl_file_size;
} fw64SoundbankHeader;

fw64SoundBank* fw64_sound_bank_load(fw64AssetDatabase* assets, uint32_t asset_id, fw64Allocator* allocator) {
    (void)assets;
    if (!allocator) allocator = fw64_default_allocator();

    int handle = fw64_filesystem_open(asset_id);

    if (handle < 0)
        return NULL;

    fw64SoundbankHeader header;
    fw64_filesystem_read(&header, sizeof(fw64SoundbankHeader), 1, handle);
    fw64_filesystem_close(handle);

    uint32_t rom_address = fw64_n64_filesystem_get_rom_address(asset_id);
    uint32_t ctrl_file_address = rom_address + sizeof(fw64SoundbankHeader);
    uint32_t tbl_file_address = ctrl_file_address + header.ctrl_file_size;

    fw64SoundBank* sound_bank = allocator->malloc(allocator, sizeof(fw64SoundBank));

    sound_bank->song_count = header.song_count;
    sound_bank->ctrl_file_size = header.ctrl_file_size;
    sound_bank->ctrl_file_address = ctrl_file_address;
    sound_bank->tbl_file_address = tbl_file_address;

    return sound_bank;
}

void fw64_sound_bank_delete(fw64AssetDatabase* assets, fw64SoundBank* sound_bank, fw64Allocator* allocator) {
    (void)assets;
    if (!allocator) allocator = fw64_default_allocator();

    allocator->free(allocator, sound_bank);
}

typedef struct {
    uint32_t track_count;
    uint32_t bank_asset_id;
    uint32_t sequence_bank_size; // this is currently unused
} fw64MusicHeader;

fw64MusicBank* fw64_music_bank_load(fw64AssetDatabase* assets, uint32_t asset_id, fw64Allocator* allocator) {
    (void)assets;
    if (!allocator) allocator = fw64_default_allocator();

    // load the music bank header which contains sequence info and also the asset ID of the sound bank
    int handle = fw64_filesystem_open(asset_id);

    if (handle < 0)
        return NULL;

    fw64MusicHeader music_header;
    fw64_filesystem_read(&music_header, sizeof(fw64MusicHeader), 1, handle);
    fw64_filesystem_close(handle);

    // get the rom address of the sequence file which contains the compressed midi files
    uint32_t rom_address = fw64_n64_filesystem_get_rom_address(asset_id);
    uint32_t seq_file_address = rom_address + sizeof(fw64MusicHeader);

    // read the associated Sound bank for the music
    handle = fw64_filesystem_open(music_header.bank_asset_id);

    if (handle < 0)
        return NULL;

    fw64SoundbankHeader soundbank_header;
    fw64_filesystem_read(&soundbank_header, sizeof(fw64SoundbankHeader), 1, handle);
    fw64_filesystem_close(handle);

    // calculate the offsets for the ctrl & table file which hold the MIDI note / sample data
    rom_address = fw64_n64_filesystem_get_rom_address(music_header.bank_asset_id);
    uint32_t ctrl_file_address = rom_address + sizeof(fw64SoundbankHeader);
    uint32_t tbl_file_address = ctrl_file_address + soundbank_header.ctrl_file_size;

    fw64MusicBank* music_bank = allocator->malloc(allocator, sizeof(fw64MusicBank));
    music_bank->track_count = music_header.track_count;
    music_bank->seq_file_address = seq_file_address;

    music_bank->ctrl_file_address = ctrl_file_address;
    music_bank->ctrl_file_size = soundbank_header.ctrl_file_size;
    music_bank->tbl_file_address = tbl_file_address;

    return music_bank;
}

void fw64_music_bank_delete(fw64AssetDatabase* assets, fw64MusicBank* music_bank, fw64Allocator* allocator) {
    (void)assets;
    if (!allocator) allocator = fw64_default_allocator();

    allocator->free(allocator, music_bank);
}
