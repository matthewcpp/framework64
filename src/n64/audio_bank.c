#include "framework64/n64/audio_bank.h"

#include <stddef.h>
#include <malloc.h>

fw64SoundBank* fw64_n64_sound_bank_load(uint32_t index) {
    fw64SoundBank* sound_bank = malloc(sizeof(fw64SoundBank));
    sound_bank->index = index;

    return sound_bank;
}