#pragma once

#include "framework64/audio.h"
#include "framework64/desktop/audio_bank.h"

#include <SDL2/SDL_mixer.h>

#include <string>
#include <vector>

struct fw64Audio {
public:
    void setSoundBank(fw64SoundBank* sb);
    int playSound(uint32_t sound_num);
    void stopSound(uint32_t channel_num);
    fw64AudioStatus getChannelStatus(uint32_t channel_num);
    size_t soundCount();

    void setMusicBank(fw64MusicBank* mb);
    bool setCurrentMusicTrack(uint32_t track_num);
    void stopMusic();
    int musicTrackCount();

private:
    void unloadCurrentMusicTrack();
    void unloadSoundBank();

private:
    fw64SoundBank* sound_bank = nullptr;
    std::vector<Mix_Chunk *> current_sounds;

    fw64MusicBank* music_bank = nullptr;
    uint32_t current_track_index = 0;
    Mix_Music* current_music_track = nullptr;

    std::string asset_dir;
};
