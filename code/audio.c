#include "audio.h"

void audio_frame_init(Audio* audio) {
    for(i32 i = 0; i < AUDIO_WAVE_COUNT; i++) {
        AudioWaveChannel* wave = &audio->wave_channels[i];
        wave->amp  = 0.0f;
        wave->freq = 0.0f;
    }
    for(i32 i = 0; i < AUDIO_NOISE_COUNT; i++) {
        AudioNoiseChannel* noise = &audio->noise_channels[i];
        noise->amp         = 0.0f;
        noise->phase_amp   = 0.0f;
        noise->phase_timer = AUDIO_NOISE_PHASE_COOLDOWN;
    }
}


