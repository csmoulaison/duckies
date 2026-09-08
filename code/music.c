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
    bool sustain_previous;
    f64 attack;
    f64 decay;
    f64 sustain;
    f64 release;
} MusicTrackNote;

typedef struct {
    MusicTrackNote* notes;
    i32             len;
    f32             attack;
    f32             decay;
    f32             sustain;
    f32             release;
    f32             max_amp;
    f32             vibrato;
} MusicTrackInfo;

f64 note(f64 note, i32 octave) {
    for(i32 i = 0; i < octave; i++) {
        note *= 2;
    }
    return (f64)note;
}

typedef struct {
    MusicTrackNote note;
    MusicTrackNote prev;
    i32 length;
    f32 t;
} NoteInfo;

#include "music_tracks.c"

NoteInfo note_info_from_track(MusicTrackInfo track, f32 t) {
    // notes cut t into thirds
    f32 track_t = fmod(t * 3.0, (f32)track.len); 
    i32 i = ((i64)(t * 3.0) % track.len);

    MusicTrackNote note = track.notes[i];
    MusicTrackNote prev_note = track.notes[true_mod((i - 1), track.len)];
    i32 note_length = 1;
    if(note.sustain_previous) {
        MusicTrackNote prev = note;
        while(prev.sustain_previous) {
            prev = track.notes[true_mod((i - note_length), track.len)];
            note_length++;
        }
    }
    f32 note_t = fmod(track_t - i + (note_length - 1), note_length);
    //f32 note_t = track_t - i + (note_length - 1);
    return (NoteInfo){note, prev_note, note_length, note_t};
}

f32 amp_from_track(MusicTrackInfo track, NoteInfo info) {
    f32 attack = track.attack;
    if(info.note.attack != 0.0) {
        attack = info.note.attack;
    }

    f32 amp = 0.0;
    f32 attenuated_amp = track.max_amp * info.note.amp;
    f32 attenuated_sustain = track.sustain * info.note.amp;
    if(info.t < attack) {
        amp = lerp(0.0, attenuated_amp, info.t / attack);
    } else if(info.t - attack < track.decay) {
        amp = lerp(attenuated_amp, attenuated_sustain, (info.t - attack) / track.decay);
    } else {
        amp = track.sustain;
    }

    if(info.note.amp == 0.0) {
        if(info.t < track.release && info.prev.amp != 0.0) {
            amp = lerp(track.sustain, 0.0, info.t / track.release);
        } else {
            amp = 0.0;
        }
    }
    return amp;
}

void wave_from_track(AudioWaveChannel* wave, MusicTrackInfo track, f32 t) {
    NoteInfo info = note_info_from_track(track, t);
    wave->amp = amp_from_track(track, info);
    f32 vib_t = info.t / 2.0;
    if(vib_t > 1.0) {
        vib_t = 1.0f;
    }
    wave->freq = info.note.freq + sinf(vib_t * vib_t * 8.0) * track.vibrato * vib_t;
}

void noise_from_track(AudioNoiseChannel* noise, MusicTrackInfo track, f32 t) {
    NoteInfo info = note_info_from_track(track, t);
    noise->amp = amp_from_track(track, info);
}

f32 wind_amp(Game* game) {
    f32 amp = 0.0;
    amp = (sin(game->time + sin(game->time / 1.0)) + 1.0) * 0.008;
    amp += (sin(game->time * 0.2 + sin(game->time / 2.0)) + 1.0) * 0.002;
    //noise->amp  = clamp(noise->amp, 0.0, 0.05) * 0.2;
    amp += 0.01;
    amp *= clamp(game->transition_t * 6.0, 0.0, 1.0);
    return amp;
}

// This happens all the time, so mustn't be dependant on any state.
// This both plays music and updates the ticks for it.
void update_music_state(Game* game, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
	MusicTrackNote bass_track[48] = { 
        { note(N_DN, 2), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 3), 2.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_GN, 3), 2.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 2), 2.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 2), 2.0, false },

        { note(N_DN, 2), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 3), 2.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_GN, 3), 2.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 2), 2.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 2), 2.0, false },

        { note(N_DN, 2), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_EN, 3), 3.0, false, 2.0 },
        { note(N_EN, 3), 3.0, true,  2.0 },
        { note(N_EN, 3), 3.0, true,  2.0 },
        { note(N_DN, 3), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_GN, 3), 3.0, false, 2.0 },
        { note(N_GN, 3), 3.0, true, 2.0 },
        { note(N_GN, 3), 3.0, true, 2.0 },

        { note(N_FN, 3), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_BN, 3), 3.0, false, 2.0 },
        { note(N_BN, 3), 3.0, true, 2.0 },
        { note(N_BN, 3), 3.0, true, 2.0 },
        { note(N_AN, 3), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 3), 2.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
	};
	MusicTrackNote melody_track[48] = { 
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_DN, 5), 1.0, false },
        { note(N_DN, 5), 1.5, false },
        { note(N_CS, 5), 1.0, false },
        { note(N_BN, 4), 1.0, false },
        { note(N_CS, 5), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_DN, 5), 1.0, false },
        { note(N_EN, 5), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },

        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_DN, 5), 1.0, false },
        { note(N_DN, 5), 1.5, false },
        { note(N_CS, 5), 1.0, false },
        { note(N_BN, 4), 1.0, false },
        { note(N_CS, 5), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_DN, 5), 1.0, false },
        { note(N_EN, 5), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },

        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_FN, 5), 0.0, false },
        { note(N_AN, 5), 1.2, false, 1.5 },
        { note(N_AN, 5), 1.3, true, 1.5},
        { note(N_GN, 5), 1.0, false },
        { note(N_FN, 5), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_AN, 5), 1.0, false },
        { note(N_CN, 6), 1.3, false, 1.5 },
        { note(N_CN, 6), 1.3, true, 1.5 },
        { note(N_AS, 5), 1.0, false },

        { note(N_AN, 5), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N_CN, 6), 1.0, false },
        { note(N_EN, 6), 1.3, false, 1.5 },
        { note(N_EN, 6), 1.5, true, 1.5 },
        { note(N_DN, 6), 1.0, false },
        { note(N_AN, 5), 1.0, false, .attack = 1.0, .release = 1.0 },
        { note(N_AS, 5), 1.0, false, .attack = 1.0, .release = 1.0 },
        { note(N_AN, 5), 1.0, false, .attack = 1.0, .release = 1.0 },
        { note(N_GN, 5), 1.0, false, .attack = 1.0, .release = 1.0 },
        { note(N_FN, 5), 1.0, false, .attack = 1.0, .release = 1.0 },
        { note(N_EN, 5), 1.0, false, .attack = 1.0, .release = 1.0 },
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
        }
    }
    for(i32 i = 0; i < 48; i++) {
       bass_track[i].freq *= 2;
    }
    AudioWaveChannel* bass = &audio->wave_channels[0];
    MusicTrackInfo bass_info = {
        .notes   = bass_track,
        .len     = 48,
        .attack  = 0.5,
        .decay   = 0.4,
        .sustain = 0.15,
        .release = 0.3,
        .max_amp = 0.3,
        .vibrato = 1.5
    };

    AudioWaveChannel* melody = &audio->wave_channels[1];
    MusicTrackInfo melody_info = {
        .notes   = melody_track,
        .len     = 48,
        .attack  = 0.3,
        .decay   = 0.35,
        .sustain = 0.2,
        .release = 0.1,
        .max_amp = 0.5,
        .vibrato = 32.0
    };
    //melody->freq = melody_track[i].freq + sinf((f32)game->frames_since_init * 1.0f) * melody_vibrato;
    //melody->amp  = (melody_amp - (i_phase * melody_amp) + sinf(i_phase) * 0.2f) * melody_track[i].amp;

    AudioNoiseChannel* noise = &audio->noise_channels[0];
    f32 move_t = fmod(game->time, 1.0f);
	MusicTrackNote drum_track[12] = { 
        { note(N000, 0), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 1.0, false },
        { note(N000, 0), 0.0, false },
        { note(N000, 0), 0.4, false }
	};
    MusicTrackInfo drum_info = {
        .notes   = drum_track,
        .len     = 12,
        .attack  = 0.1,
        .decay   = 0.1,
        .sustain = 0.01,
        .release = 0.2,
        .max_amp = 0.35,
        .vibrato = 0.0
    };

    if(hannah_moved_this_cycle) {
        drum_info.decay += 0.075;
        drum_info.sustain = 0.1;
        drum_info.max_amp += 0.01;

        melody_info.decay *= 1.5;
        melody_info.sustain *= 1.25;
        bass_info.sustain *= 1.25;
    }

    if(game->mode == MODE_MENU || game->mode == MODE_MENU_TO_GAME) {
        melody_info.max_amp = 0.0;
        melody_info.sustain = 0.0;
        bass_info.attack = 1.5;
        bass_info.decay = 0.1;
        bass_info.sustain = 0.0;
    }


    noise_from_track(noise, drum_info, game->time);
    wave_from_track(bass, bass_info, game->time);
    wave_from_track(melody, melody_info, game->time);

    switch(game->music_override_state) {
        case MUSIC_OVERRIDE_MENU_FADE: {
            melody->amp = lerp(melody->amp, 0.0, game->transition_t);
            melody->amp = 0.0;
            bass->amp   = lerp(bass->amp,   0.0, game->transition_t);
            noise->amp  = lerp(noise->amp,  0.0, game->transition_t);
            if(game->transition_t >= 1.0) {
                bass->amp = 0.0;
                noise->amp = 0.0;
            }
        } break;
        case MUSIC_OVERRIDE_WINDY: {
            melody->amp = 0.0;
            bass->amp   = 0.0;
            noise->amp = wind_amp(game);
        } break;
        case MUSIC_OVERRIDE_WINDY_DRUMS: {
            melody->amp = 0.0;
            bass->amp   = 0.0;
            noise->amp += wind_amp(game);
        }
        default: break;
    }

    if(game->mute) {
        melody->amp = 0.0;
        bass->amp = 0.0;
        noise->amp = 0.0;
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
    sfx->amp = 0.0;

    if(moving_platform_this_cycle && move_t < 0.25) {
        sfx->amp  = 0.05 + move_t * 0.10;
        sfx->freq = 150.0 + 300.0 * move_t;
    }

    bool duck_honking = false;
    for(i32 i = 0; i < ducks_len(state); i++) {
        Entity* duck = &state->ducks[i];
        if(duck->honk_this_cycle) {
            duck_honking = true;
            break;
        }
    }
    if(move_t < 0.33) {
        NoteInfo bass_note = note_info_from_track(bass_info, game->time);
        if(duck_honking) {
            sfx->amp = 0.75 * move_t + sin(game->time * 75.0) * 0.075;
            sfx->freq = bass_note.note.freq * 0.5 + 0.0 * move_t;
            if(sfx->freq > 100.0) {
                sfx->freq *= 0.5;
            }
        } else if(state->marchers[0].honk_this_cycle) {
            sfx->amp = 0.75 * move_t;
            sfx->freq = bass_note.note.freq * 2.0 + 0.0 * move_t;
            if(sfx->freq > 400.0) {
                sfx->freq *= 0.5;
            }
        }
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

    // NOW: How does the continuous snake cycle work? Levels can request they be
    // reset to a particular cycle (probably 16 for snake), and when level
    // resets occur, the level switch fade holds until the music can reset on
    // a 4ish beat boundary.

    i64 t_old = (i64)(game->time * 2.0f);
	game->time += TIME_SCALE * dt;
    i64 t_new = (i64)(game->time * 2.0f);
    if((t_old % 2 == 0 && t_new % 2 == 1) || (t_old % 2 == 1 && t_new % 2 == 0)) {
        game->cycle_stage_counter++;
        if(game->cycle_stage_counter > 1) {
            game->cycle_index++;
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

