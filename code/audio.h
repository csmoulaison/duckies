#ifndef audio_h_INCLUDED
#define audio_h_INCLUDED

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_WAVE_COUNT 3
#define AUDIO_NOISE_COUNT 1
#define AUDIO_NOISE_PHASE_COOLDOWN 8

u8 triangle_lut[32] = { 
    0,
    0 + 16,
    32,
    32 + 16,
    64,
    64 + 16,
    96,
    96 + 16,
    128,
    128 + 16,
    160,
    160 + 16,
    192,
    192 + 16,
    224,
    224 + 16,
    255,
    255 - 16,
    224,
    224 - 16,
    192,
    192 - 16,
    160,
    160 - 16,
    128,
    128 - 16,
    96,
    96 - 16,
    64,
    64 - 16,
    32,
    16
};

typedef struct {
    f64 amp;
    f64 freq;
    f64 phase;
    f64 amp_actual;
    f64 freq_actual;;
} AudioWaveChannel;

typedef struct {
    f64 amp;
    f64 phase_amp;
    i8  phase_timer;
} AudioNoiseChannel;

typedef struct {
    f64               attenuation;
    f64               shelf;
    AudioWaveChannel  wave_channels[AUDIO_WAVE_COUNT];
    AudioNoiseChannel noise_channels[AUDIO_NOISE_COUNT];
} Audio;

void audio_init(Audio* audio) {
    memset(audio, 0, sizeof(Audio));
    audio->attenuation = 0.6f;
    audio->shelf = 0.9f;
}

void audio_callback(void* userdata, u8* buffer, i32 len) {
    Audio* audio = (Audio*)userdata;
    f32* stream = (f32*)buffer;
    AudioWaveChannel* wave;
    AudioNoiseChannel* noise;

    for(i32 i = 0; i < AUDIO_WAVE_COUNT; i++) {
        wave = &audio->wave_channels[i];
        if(wave->amp < 0.0f) {
            wave->amp = 0.0f;
        }
        if(wave->freq < 0.0f) {
            wave->freq = 0.0f;
        }
    }
    for(i32 i = 0; i < AUDIO_NOISE_COUNT; i++) {
        noise = &audio->noise_channels[i];
        if(noise->amp < 0.0f) {
            noise->amp = 0.0f;
        }
    }
    
    for(i32 i = 0; i < len / 4; i++) {
		stream[i] = 0.0f;
        for(i32 j = 0; j < AUDIO_WAVE_COUNT; j++) {
            wave = &audio->wave_channels[j];
            if(wave->freq > 10.0f) {
                wave->freq_actual = lerp(wave->freq_actual, wave->freq, 0.002f);
            }
            wave->amp_actual  = lerp(wave->amp_actual,  wave->amp,  0.002f);
            wave->phase += 2.0f * M_PI * wave->freq_actual / AUDIO_SAMPLE_RATE;

            // sin
            //stream[i] += wave->amp_actual * sinf(wave->phase);

            // triangle
            stream[i] += wave->amp_actual * ((f32)(triangle_lut[(i64)(wave->phase * 4) % 32]) / 256.0f);
        }
        for(i32 j = 0; j < AUDIO_NOISE_COUNT; j++) {
            noise = &audio->noise_channels[j];
            if(noise->phase_timer < 0) {
    			noise->phase_timer = AUDIO_NOISE_PHASE_COOLDOWN;
    			noise->phase_amp = random_f32_signed() * noise->amp;
            }
            noise->phase_timer--;
			stream[i] += noise->phase_amp;
        }
		stream[i] *= audio->attenuation;
		// RELEASE: remove this assert
		assert(stream[i] > -audio->shelf && stream[i] < audio->shelf);
		stream[i] = clamp(stream[i], -audio->shelf, audio->shelf);
    }
}

#endif
