#include "framework64/desktop/audio.hpp"

#include "framework64/desktop/asset_database.hpp"

#include <SDL2/SDL_mixer.h>

bool fw64Audio::loadSoundBankAsset(fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    auto asset_path = asset_database->getAssetPath(asset_id);
    if (asset_path.empty() || asset_path == sound_bank_path) {
        return false;
    }

    unloadSoundBank();

    auto bank_info_path = asset_path / "info.soundbank";
    framework64::FileDataSource datasource;
    if (!datasource.open(bank_info_path)) {
        return false;
    }

    sound_bank_path = std::move(asset_path);

    uint32_t sound_count;
    fw64_data_source_read(&datasource.interface, &sound_count, sizeof(uint32_t), 1);
    

    current_sounds.resize(sound_count, nullptr);
    for (size_t i = 0; i < current_sounds.size(); i++) {
        auto sound_path = sound_bank_path / (std::to_string(i) + ".ogg");

        auto* sound_effect = Mix_LoadWAV(sound_path.string().c_str());

        if (!sound_effect) {
            unloadSoundBank();
            return false;
        }

        current_sounds[i] = sound_effect;
    }
    return true;
}

void fw64Audio::unloadSoundBank() {
    for (const auto sound_effect : current_sounds) {
        if (sound_effect)
            Mix_FreeChunk(sound_effect);
    }

    current_sounds.resize(0);
}

int fw64Audio::playSound(uint32_t sound_num) {
    if (sound_num >= current_sounds.size())
        return -1;

    return Mix_PlayChannel(-1, current_sounds[sound_num], 0);
}

void fw64Audio::stopSound(uint32_t channel_num) {
    Mix_HaltChannel(channel_num);
}

fw64AudioStatus fw64Audio::getChannelStatus(uint32_t channel_num) {
    if (Mix_Playing(channel_num))
        return FW64_AUDIO_PLAYING;
    else
        return FW64_AUDIO_STOPPED;
}

bool fw64Audio::loadMusicBankAsset(fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    auto asset_path = asset_database->getAssetPath(asset_id);
    if (asset_path.empty() || asset_path == sound_bank_path) {
        return false;
    }

    unloadMusicBank();

    auto bank_info_path = asset_path / "info.musicbank";
    framework64::FileDataSource datasource;
    if (!datasource.open(bank_info_path)) {
        return false;
    }

    music_bank_path = std::move(asset_path);

    fw64_data_source_read(&datasource.interface, &music_bank_size, sizeof(uint32_t), 1);
    
    return true;
}

void fw64Audio::unloadMusicBank() {
    unloadCurrentMusicTrack();

    music_bank_path.clear();
    music_bank_size = 0;
    current_track_index = FW64_DESKTOP_AUDIO_INVALID_TRACK_INDEX;
}

void fw64Audio::unloadCurrentMusicTrack() {
    if (current_music_track) {
        Mix_HaltMusic();
        Mix_FreeMusic(current_music_track);
        current_music_track = nullptr;
    }
}

bool fw64Audio::setCurrentMusicTrack(uint32_t track_num) {
    if (track_num >= music_bank_size)
        return false;

    if (current_track_index != track_num) {
        unloadCurrentMusicTrack();

        auto music_track_path = music_bank_path / (std::to_string(track_num) + ".ogg");
        current_music_track = Mix_LoadMUS(music_track_path.string().c_str());
    }

    if (current_music_track) {
        current_track_index = track_num;
        Mix_PlayMusic(current_music_track, 0);
        return true;
    }
    else {
        return false;
    }
}

void fw64Audio::stopMusic() {
    Mix_HaltMusic();
}

// C Interface -> sounds

int fw64_audio_load_soundbank_asset(fw64Audio* audio, fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    return static_cast<int>(audio->loadSoundBankAsset(asset_database, asset_id));
}

void fw64_audio_unload_soundbank(fw64Audio* audio) {
    audio->unloadSoundBank();
}

uint32_t fw64_audio_sound_count(fw64Audio* audio) {
    return static_cast<uint32_t>(audio->soundCount());
}

int fw64_audio_play_sound(fw64Audio* audio, uint32_t sound_num) {
    return audio->playSound(sound_num);
}

void fw64_audio_stop_sound(fw64Audio* audio, int handle) {
    audio->stopSound(handle);
}

fw64AudioStatus fw64_audio_get_sound_status(fw64Audio* audio, int handle) {
    return audio->getChannelStatus(handle);
}

// C Interface -> music

int fw64_audio_load_musicbank_asset(fw64Audio* audio, fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    return static_cast<int>(audio->loadMusicBankAsset(asset_database, asset_id));
}

void fw64_audio_unload_musicbank(fw64Audio* audio) {
    return audio->unloadMusicBank();
}

int fw64_audio_play_music(fw64Audio* audio, uint32_t track_num) {
    return audio->setCurrentMusicTrack(track_num) ? 1 : 0;
}

void fw64_audio_stop_music(fw64Audio* audio) {
    audio->stopMusic();
}

void fw64_audio_set_music_volume(fw64Audio* audio, float volume) {
    audio->music_volume = volume;
    Mix_VolumeMusic(static_cast<int>(volume * MIX_MAX_VOLUME));
}

uint32_t fw64_audio_music_track_count(fw64Audio* audio) {
    return static_cast<uint32_t>(audio->musicTrackCount());
}

fw64AudioStatus fw64_audio_get_music_status(fw64Audio*) {
    return Mix_PlayingMusic() ? FW64_AUDIO_PLAYING : FW64_AUDIO_STOPPED;
}

float fw64_audio_get_playback_speed(fw64Audio* audio) {
    return audio->music_playback_speed;
}

void fw64_audio_set_music_playback_speed(fw64Audio* audio, float speed) {
    audio->music_playback_speed = speed;
}