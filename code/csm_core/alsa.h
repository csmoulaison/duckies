#ifndef alsa_h_INCLUDED
#define alsa_h_INCLUDED

#include <alsa/asoundlib.h>
#include <alloca.h>

#define ALSA_BUFFER_SAMPLES_MAX 2048

#define ALSA_VERIFY(alsa_function) { \
	i32 alsa_error; \
	if((alsa_error = alsa_function) < 0) { \
		fprintf(stderr, "ALSA error: %s\n", snd_strerror(alsa_error)); \
		exit(1); \
	} \
} 

typedef struct {
	snd_pcm_t* pcm;
	u8         write_buffer[ALSA_BUFFER_SAMPLES_MAX];
	u32        latency_samples;
} AlsaDevice;

void alsa_init(AlsaDevice* alsa, u32 sample_rate);
i32  alsa_write_samples_count(AlsaDevice* alsa);
void alsa_write_samples(AlsaDevice* alsa, i16* buffer, i32 sample_count);

#ifdef CSM_IMPLEMENTATION

void alsa_init(AlsaDevice* alsa, u32 sample_rate) {
	snd_pcm_hw_params_t* hw_params;

	ALSA_VERIFY(snd_pcm_open(&alsa->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0));
	snd_pcm_hw_params_alloca(&hw_params);
	ALSA_VERIFY(snd_pcm_hw_params_any(alsa->pcm, hw_params));
	ALSA_VERIFY(snd_pcm_hw_params_set_access(alsa->pcm, hw_params, SND_PCM_ACCESS_RW_NONINTERLEAVED));
	//ALSA_VERIFY(snd_pcm_hw_params_set_format(alsa->pcm, hw_params, SND_PCM_FORMAT_S16_LE));
	ALSA_VERIFY(snd_pcm_hw_params_set_format(alsa->pcm, hw_params, SND_PCM_FORMAT_FLOAT_LE));
	ALSA_VERIFY(snd_pcm_hw_params_set_rate_near(alsa->pcm, hw_params, &sample_rate, 0));
	ALSA_VERIFY(snd_pcm_hw_params_set_channels(alsa->pcm, hw_params, 1));
	ALSA_VERIFY(snd_pcm_hw_params(alsa->pcm, hw_params));
	ALSA_VERIFY(snd_pcm_prepare(alsa->pcm));
	alsa->latency_samples = sample_rate / 8;
}

i32 alsa_write_samples_count(AlsaDevice* alsa) {
	snd_pcm_sframes_t available;
	snd_pcm_sframes_t delay;
	ALSA_VERIFY(snd_pcm_avail_delay(alsa->pcm, &available, &delay));
	// TODO: Make sure we have enough frames available.
	return alsa->latency_samples - delay;
}

void alsa_write_samples_normalized(AlsaDevice* alsa, f32* fbuffer, i32 sample_count) {
    /*
    i16 ibuffer[sample_count] = {};
    for(i32 i = 0; i < sample_count; i++) {
        ibuffer[i] = (i16)clamp(fbuffer[i] * 65000.0f, -65000.0f, 65000.0f);
    }
    alsa_write_samples(alsa, ibuffer, sample_count);
    */

    i32 frames_written = snd_pcm_writen(alsa->pcm, (void**)&fbuffer, sample_count);
    //printf("frames %d samples %d\n", frames_written, sample_count);
    assert(frames_written == sample_count);
}

void alsa_write_samples(AlsaDevice* alsa, i16* buffer, i32 sample_count) {
    i32 frames_written = snd_pcm_writen(alsa->pcm, (void**)&buffer, sample_count);
    printf("frames %d samples %d\n", frames_written, sample_count);
    assert(frames_written == sample_count);
}

#endif
#endif 
