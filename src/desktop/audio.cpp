#include "framework64/desktop/audio.h"

void fw64Audio::setSoundBank(fw64SoundBank* sb) {
    if (sound_bank == sb)
        return;

    if (sound_bank)
        unloadSoundBank();

    sound_bank = sb;
    current_sounds.resize(sound_bank->sound_count, nullptr);

    for (size_t i = 0; i < current_sounds.size(); i++) {
        std::string sound_path = sound_bank->base_path + std::to_string(i) + ".ogg";

        auto* sound_effect = Mix_LoadWAV(sound_path.c_str());

        if (!sound_effect) {
            unloadSoundBank();
            break;
        }

        current_sounds[i] = sound_effect;
    }
}

int fw64Audio::playSound(uint32_t sound_num) {
    if (!sound_bank || sound_num >= current_sounds.size())
        return -1;

    return Mix_PlayChannel(-1, current_sounds[sound_num], 0);
}

void fw64Audio::stopSound(uint32_t channel_num) {
    Mix_HaltChannel(channel_num);
}

size_t fw64Audio::soundCount() {
    return current_sounds.size();
}

fw64AudioStatus fw64Audio::getChannelStatus(uint32_t channel_num) {
    if (Mix_Playing(channel_num))
        return FW64_AUDIO_PLAYING;
    else
        return FW64_AUDIO_STOPPED;
}

void fw64Audio::setMusicBank(fw64MusicBank* mb) {
    unloadCurrentMusicTrack();
    music_bank = mb;
}

void fw64Audio::unloadCurrentMusicTrack() {
    if (current_music_track) {
        Mix_HaltMusic();
        Mix_FreeMusic(current_music_track);
        current_music_track = nullptr;
    }
}

void fw64Audio::unloadSoundBank() {
    for (const auto sound_effect : current_sounds) {
        if (sound_effect)
            Mix_FreeChunk(sound_effect);
    }

    current_sounds.resize(0);
}

bool fw64Audio::setCurrentMusicTrack(uint32_t track_num) {
    if (!music_bank)
        return false;

    
    if (current_music_track && Mix_PlayingMusic() && current_track_index == track_num)
        return false;

    if (track_num >= music_bank->track_count)
        return false;

    unloadCurrentMusicTrack();

    std::string file = music_bank->base_path + std::to_string(track_num) + ".ogg";
    current_music_track = Mix_LoadMUS(file.c_str());

    if (current_music_track) {
        current_track_index = track_num;
        Mix_PlayMusic(current_music_track, 0);
        return true;
    }
    else {
        return false;
    }
}

int fw64Audio::musicTrackCount() {
    return (music_bank) ? static_cast<int>(music_bank->track_count) : 0;
}

void fw64Audio::stopMusic() {
    Mix_HaltMusic();
}

// C Interface

void fw64_audio_set_sound_bank(fw64Audio* audio, fw64SoundBank* sound_bank) {
    audio->setSoundBank(sound_bank);
}

int fw64_audio_sound_count(fw64Audio* audio) {
    return static_cast<int>(audio->soundCount());
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

void fw64_audio_set_music_bank(fw64Audio* audio, fw64MusicBank* music_bank) {
    audio->setMusicBank(music_bank);
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

int fw64_audio_music_track_count(fw64Audio* audio) {
    return audio->musicTrackCount();
}

fw64AudioStatus fw64_audio_get_music_status(fw64Audio* audio) {
    return Mix_PlayingMusic() ? FW64_AUDIO_PLAYING : FW64_AUDIO_STOPPED;
}

float fw64_audio_get_playback_speed(fw64Audio* audio) {
    return audio->music_playback_speed;
}

void fw64_audio_set_music_playback_speed(fw64Audio* audio, float speed) {
    audio->music_playback_speed = speed;
}