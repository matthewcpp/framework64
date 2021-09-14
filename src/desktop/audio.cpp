#include "framework64/desktop/audio.h"

#include <SDL2/SDL_mixer.h>

void fw64Audio::setSoundBank(fw64SoundBank* sb) {
    sound_bank = sb;
}

int fw64Audio::playSound(uint32_t sound_num) {
    if (!sound_bank || sound_num >= sound_bank->sounds.size())
        return -1;


    return Mix_PlayChannel(-1, sound_bank->sounds[sound_num], 0);
}

void fw64Audio::stopSound(uint32_t channel_num) {
    Mix_HaltChannel(channel_num);
}

size_t fw64Audio::soundCount() {
    if (sound_bank)
        return sound_bank->sounds.size();
    else
        return 0;
}

fw64AudioStatus fw64Audio::getChannelStatus(uint32_t channel_num) {
    if (Mix_Playing(channel_num))
        return FW64_AUDIO_PLAYING;
    else
        return FW64_AUDIO_STOPPED;
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

}

int fw64_audio_play_music(fw64Audio* audio, uint32_t track_num) {
    return 0;
}

void fw64_audio_stop_music(fw64Audio* audio) {

}

void fw64_audio_set_music_volume(fw64Audio* audio, float volume) {

}

int fw64_audio_music_track_count(fw64Audio* audio) {
    return 0;
}

fw64AudioStatus fw64_audio_get_music_status(fw64Audio* audio) {
    return FW64_AUDIO_STOPPED;
}