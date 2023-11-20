#pragma once

#include "framework64/audio.h"

#include <SDL2/SDL_mixer.h>

#include <filesystem>
#include <limits>
#include <string>
#include <vector>

#define FW64_DESKTOP_AUDIO_INVALID_TRACK_INDEX std::numeric_limits<uint32_t>::max()

struct fw64Audio {
public: // sounds
    bool loadSoundBankAsset(fw64AssetDatabase* asset_database, fw64AssetId asset_id);
    void unloadSoundBank();

    int playSound(uint32_t sound_num);
    void stopSound(uint32_t channel_num);
    fw64AudioStatus getChannelStatus(uint32_t channel_num);

    inline size_t soundCount() const {
        return current_sounds.size();
    }

public: //music
    bool loadMusicBankAsset(fw64AssetDatabase* asset_database, fw64AssetId asset_id);
    void unloadMusicBank();

    bool setCurrentMusicTrack(uint32_t track_num);
    void stopMusic();

    inline int musicTrackCount() const {
        return music_bank_size;
    }

    float music_playback_speed = 1.0f;
    float music_volume = 1.0f;

private:
    void unloadCurrentMusicTrack();

private:
    std::filesystem::path sound_bank_path;
    std::vector<Mix_Chunk *> current_sounds;

    std::filesystem::path music_bank_path;
    uint32_t music_bank_size = 0;
    uint32_t current_track_index = FW64_DESKTOP_AUDIO_INVALID_TRACK_INDEX;
    Mix_Music* current_music_track = nullptr;
};
