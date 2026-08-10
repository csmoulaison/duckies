#define N000 00.00
#define N_CN 16.35
#define N_CS 17.32
#define N_DN 18.35
#define N_DS 19.45
#define N_EN 20.60
#define N_FN 21.83
#define N_FS 23.12
#define N_GN 24.50
#define N_GS 25.96
#define N_AN 27.50
#define N_AS 29.14
#define N_BN 30.87

// TODO: try these ideas for flute sounds. These are notes for the NES chip.
// 
// The "Breath" Attack (Volume Envelope): Real players cannot instantly hit
// maximum volume. For pulse wave woodwinds, create a volume envelope that
// starts at volume level 4, ramps up to 15 over a few frames, and tapers off
// gracefully.
// 
// Legato & Portamento (Sliding Notes): Wind players frequently slur notes
// together without breaking their breath. Use pitch-glide commands to smoothly
// bend the pitch between consecutive notes rather than re-triggering them
// sharply.
//
// Vibrato: Wind musicians use their embouchure or diaphragm to create pitch
// variance. Program a gentle pitch vibrato that kicks in slightly after a note
// is held down, rather than immediately at the start of the note.
// 
// Chiff / Air Sound: To add realism to the start of a flute note, rapidly play
// a single frame of low-volume white noise right as the triangle wave starts.
// This perfectly mimics the brief "chiff" of air hitting a physical embouchure
// hole.

typedef struct {
    f64 freq;
    f64 amp;
} MusicTrackNote;

f64 note(f64 note, i32 octave) {
    for(i32 i = 0; i < octave; i++) {
        note *= 2;
    }
    return (f64)note;
}

// This happens all the time, so mustn't be dependant on any state.
// This both plays music and updates the ticks for it.
void update_music_state(Game* game, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
	MusicTrackNote bass_track[48] = { 
        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_FN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_EN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 1), 2.0 },

        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_FN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_EN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 1), 2.0 },

        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_EN, 2), 2.0 },
        { note(N_EN, 2), 2.0 },
        { note(N_EN, 2), 1.0 },
        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_GN, 2), 2.0 },
        { note(N_GN, 2), 2.0 },
        { note(N_GN, 2), 1.0 },

        { note(N_FN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_BN, 2), 2.0 },
        { note(N_BN, 2), 2.0 },
        { note(N_BN, 2), 2.0 },
        { note(N_AN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
	};
	MusicTrackNote melody_track[48] = { 
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_DN, 5), 1.5 },
        { note(N_CS, 5), 1.0 },
        { note(N_BN, 4), 1.0 },
        { note(N_CS, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_EN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },

        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_DN, 5), 1.5 },
        { note(N_CS, 5), 1.0 },
        { note(N_BN, 4), 1.0 },
        { note(N_CS, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_EN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },

        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_FN, 5), 0.0 },
        { note(N_AN, 5), 1.2 },
        { note(N_AN, 5), 1.3 },
        { note(N_GN, 5), 1.0 },
        { note(N_FN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 5), 1.0 },
        { note(N_CN, 6), 1.0 },
        { note(N_CN, 6), 1.0 },
        { note(N_AS, 5), 1.0 },

        { note(N_AN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_CN, 6), 1.0 },
        { note(N_EN, 6), 1.0 },
        { note(N_EN, 6), 1.0 },
        { note(N_DN, 6), 1.0 },
        { note(N_AN, 5), 1.0 },
        { note(N_AS, 5), 1.0 },
        { note(N_AN, 5), 1.0 },
        { note(N_GN, 5), 1.0 },
        { note(N_FN, 5), 1.0 },
        { note(N_EN, 5), 1.0 },
	};
	f64 drum_track[12] = { 
        1.2f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.9f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.6f
	};

	f32 i_phase = (f32)((i64)(game->time * 36.0f) % 12) / 12.0f;
	i32 i = ((i64)(game->time * 3.0f) % 48);
	//bool hannah_moved_this_cycle = (state->hannah.move_this_cycle != MOVE_NONE);
	bool hannah_moved_this_cycle = state->hannah_manual_moved_this_cycle;

    f32 melody_vibrato = 3.0f;
    f32 melody_amp = 0.0f;
    if(hannah_moved_this_cycle) {
        melody_vibrato = 2.0f;
        melody_amp = 0.2f;
    } else {
        for(i32 i = 0; i < 48; i++) {
            bass_track[i].amp *= 0.4;
            bass_track[i].freq *= 2;
        }
    }
    AudioWaveChannel* bass = &audio->wave_channels[0];
    bass->freq = bass_track[i].freq;
    bass->amp  = bass_track[i].amp * 0.25;

    AudioWaveChannel* melody = &audio->wave_channels[1];
    melody->freq = melody_track[i].freq + sinf((f32)game->frames_since_init * 1.0f) * melody_vibrato;
    melody->amp  = (melody_amp - (i_phase * melody_amp) + sinf(i_phase) * 0.2f) * melody_track[i].amp;

    if(game->mode == MODE_MENU) {
        melody->amp = 0.0;
        bass->freq *= 0.5;
        bass->amp *= 1.2;
    }

    if(game->mute) {
        melody->amp = 0.0;
        bass->amp = 0.0;
    }

    AudioNoiseChannel* noise = &audio->noise_channels[0];
    f32 move_t = fmod(game->time, 1.0f);
    i32 drum_i = ((i64)(game->time * 3.0f) % 12);
    f32 drum_i_phase = i_phase;
    if(drum_i == 11 && !hannah_moved_this_cycle) {
    	drum_i_phase = (f32)((i64)(game->time * 72.0f) % 12) / 12.0f;
    }
    // Probs extricate some of this from the main music loop.
    noise->amp = clamp(drum_track[drum_i] * 0.166f - drum_i_phase, 0.0f, 1.0f);
    if(hannah_moved_this_cycle && move_t < 0.5f) {
        noise->amp += 0.02f + move_t * 0.10f;
    }

    // Moving platform sounds
    bool moving_platform_this_cycle = false;
    for(i32 i = 0; i < state->platforms_len; i++) {
        Entity* platform = &state->platforms[i];
        if(platform->move_this_cycle != MOVE_NONE) {
            moving_platform_this_cycle = true;
        }
    }
    AudioWaveChannel* sfx = &audio->wave_channels[2];
    if(moving_platform_this_cycle && move_t < 0.25) {
        sfx->amp  = 0.22 + move_t * 0.30;
        sfx->freq = 150.0 + 600.0 * move_t;
    } else {
        sfx->amp = 0.0;
    }

    if(game->debug_stepping && game->new_cycle_queued) {
        bass->amp   = 0.0;
        melody->amp = 0.0;
        noise->amp  = 0.0;
        if(state->input_move != MOVE_NONE || input_button_pressed(game->input_buttons[BUTTON_EDITOR_PLACE])) {
            game->new_cycle_queued = false;
            game->new_cycle_this_frame = true;
        }
        return;
    }

    i64 t_old = (i64)(game->time * 2.0f);
	game->time += TIME_SCALE * dt;
    i64 t_new = (i64)(game->time * 2.0f);
    if((t_old % 2 == 0 && t_new % 2 == 1) || (t_old % 2 == 1 && t_new % 2 == 0)) {
        game->cycle_stage_counter++;
        if(game->cycle_stage_counter > 1) {
            if(game->mode == MODE_GAME) {
                game->state.cycle_index++;
            }

            if(game->debug_stepping) {
                game->new_cycle_queued = true;
            } else {
                game->new_cycle_this_frame = true;
            }
            game->half_cycle_this_frame = false;
            state->hannah_manual_moved_this_cycle = false;
            game->cycle_stage_counter = 0;
        } else if(game->cycle_stage_counter <= 1) {
            game->half_cycle_this_frame = true;
            game->new_cycle_this_frame = false;
        }
    } else {
        game->new_cycle_this_frame = false;
        game->half_cycle_this_frame = false;
    }
}

