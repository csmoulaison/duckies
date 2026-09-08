#define CUT_RIVER_GATE_TIME 5.0
#define CUT_OPENING_TIME 9.0
#define CUT_EGG_RUMBLE_TIME 10.0

#define OPENING_XOFF -16.0

void start_cutscene(Game* game, CutsceneMode mode) {
    game->transition_t = 0.0;
    game->cutscene_mode = mode;
    game->mode = MODE_CUTSCENE;
}

f32 t_range(f32 t, f32 start, f32 end) {
    f32 range = end - start;
    f32 offset = t - start;
    return offset * (1.0 / range);
}

f32 t_in_range(f32 t, f32 start, f32 end) {
    f32 range = t_range(t, start, end);
    if(range < 0.0 || range > 1.0) {
        return false;
    } 
    return true;
}

f32 clamped_t_range(f32 t, f32 start, f32 end) {
    return clamp(t_range(t, start, end), 0.0, 1.0);
}

void draw_punctuation(Game* game, DrawList* draw_list, i64 sprite, v2 pos, f32 t, f32 start, f32 end) {
    if(t_in_range(t, start, end)) {
        draw_sprite(draw_list, SPRITE_QUESTION, 0, v2_add(pos, v2_new(0.0, lerp(0.0, 4.0, smoothstep(t_range(t, start, end))))), 0);
    }
}

void draw_opening(Game* game, DrawList* draw_list, Audio* audio, f32 t) {
    // enviro, pos offset
    v2 pos_offset = v2_zero();
    pos_offset.x += lerp(OPENING_XOFF, 0.0, smoothstep(clamped_t_range(t, 0.2, 0.4)));
    //if(t < 0.2) {
    //    pos_offset.x = OPENING_XOFF;
    //} else if(t < 0.4) {
    //    pos_offset.x = lerp(OPENING_XOFF, 0.0, smoothstep((t - 0.2) * 5.0));
    //}
    for(i32 i = 0; i < 32; i++) {
        v2 draw_pos = v2_new(64.0 + 8.0 * (i % 4), 8.0 * (i / 4));
        draw_pos = v2_add(draw_pos, pos_offset);
        draw_sprite(draw_list, SPRITE_GRASS, 0, draw_pos, 0);
    }
    draw_level_tiles(game, active_game_level(game), draw_list, pos_offset);

    // hannah
    v2 hannah_pos = v2_add(v2_new(32.0, 32.0), pos_offset);
    draw_sprite(draw_list, SPRITE_HANNAH_SIT, 0, hannah_pos, 0);

    // question
    f32 qstart = 0.43;
    f32 qend = 0.50;
    draw_punctuation(game, draw_list, SPRITE_QUESTION, v2_add(hannah_pos, v2_new(8.0, 8.0)), t, qstart, qend);
    AudioWaveChannel* wave = &audio->wave_channels[3];
    if(t_in_range(t, qstart, qend)) {
        f32 qrange = t_range(t, qstart, qend);
        wave->amp = 0.5 - qrange * 0.3;
        wave->freq = 300.0 + qrange * 1000.0;
    }

    // egg
    draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 0, v2_add(v2_new(8.0, 16.0), pos_offset), 0);
}

void mode_cutscene_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
    String msg[16];
    bool update_main = true;
    switch(game->cutscene_mode) {
        case CUT_OPENING: {
            game->transition_t += dt / CUT_OPENING_TIME;
            draw_opening(game, draw_list, audio, game->transition_t);
            game->music_override_state = MUSIC_OVERRIDE_WINDY;
            update_main = false;
            if(game->transition_t > 1.0) {
                game->mode = MODE_GAME;
                game->music_override_state = MUSIC_OVERRIDE_WINDY_DRUMS;
                //game->time = 0.0;
            }
        } break;

        case CUT_EGG_RUMBLE: {
            state->override_egg_draw = true;
            state->override_hannah_draw = true;
            game->music_override_state = MUSIC_OVERRIDE_WINDY;
            game->transition_t += dt / CUT_EGG_RUMBLE_TIME;
            if(game->transition_t > 1.0) {
                state->egg_states[state->egg_index] = EGG_BREAKING;
                game->mode = MODE_EGG_EXPLODE;
                state->egg_t = 0.0;
                state->override_egg_draw = false;
                state->override_hannah_draw = false;
                game->music_override_state = MUSIC_OVERRIDE_WINDY_DRUMS;
                break;
            }
            update_main = false;
            update_visual_state(game, draw_list, v2_zero(), dt);
            draw_sprite_animated(draw_list, SPRITE_EGG_RUMBLE, game->transition_t, v2_new(8.0, 16.0), 0);
            draw_sprite(draw_list, SPRITE_HANNAH_WAIT, 0, v2_scale(v2_from_iv2(state->hannah.pos_cur), 8.0), 0);
        } break;
        
        case CUT_FROG_PAUSE: {
            game->transition_t += dt / 2.0;
            if(game->transition_t > 1.0) {
                Msg msg[3] = {
                    new_msg(MSG_FROG, string_const("TRY CROSSING THIS ROAD.")),
                    new_msg(MSG_FROG, string_const("IT'S A REAL THRILL, AS I'M SURE YOU'LL FIND.")),
                    new_msg(MSG_FROG, string_const("WE'LL MEET UP AHEAD."))
                };
                start_msg(game, msg, 3, MODE_GAME);
            }
        } break;

        case CUT_FROG_INVITATION: {
            game->transition_t += dt / 0.75;
            if(game->transition_t > 1.0) {
                Msg msg[4] = {
                    new_msg(MSG_DUCK, string_const("HEY, MISTER FROG!")),
                    new_msg(MSG_DUCK, string_const("WE'RE ON OUR WAY TO A TEA PARTY, YOU SHOULD COME ALONG!")),
                    new_msg(MSG_DUCK, string_const("THERE WILL BE TEA... AND...")),
                    new_msg(MSG_DUCK, string_const("AND ALL THE OTHER NECESSARY TEA PARTY ACCOUTREM- ENTS!"))
                };
                start_msg_queue_cutscene(game, msg, 4, CUT_FROG_INVITATION_2);
            }
        } break;

        case CUT_FROG_INVITATION_2: {
            Msg msg[3] = {
                new_msg(MSG_FROG, string_const("TEA PARTY, YOU SAY?")),
                new_msg(MSG_FROG, string_const("RIBBIT.")),
                new_msg(MSG_FROG, string_const("I'LL BE THERE. I'LL SEE YOU AT TEA TIME."))
            };
            start_msg(game, msg, 3, MODE_GAME);
        } break;

        case CUT_RIVER_GATE: {
            update_main = false;
            game->transition_t += dt / CUT_RIVER_GATE_TIME;
            f32 t = game->transition_t;
            if(t < 0.2) {
                override_pallete_from_fade_t(draw_list, t * 5.0);
            }
            if(t > 0.8) {
                override_pallete_from_fade_t(draw_list, (t - 0.8) * 5.0);
            }
            if(t < 0.1 || t > 0.9) {
                update_main = true;
            } else {
                Level* draw_level = &game->world->levels[51];
                draw_level_tiles(game, draw_level, draw_list, v2_zero());
                draw_gate_t(draw_list, iv2_new(5, 0), lerp(-2.5, 3.5, t), GATE_OPEN, v2_zero());
            }
            if(t >= 1.0) {
                game->mode = MODE_GAME;
                game->transition_t= 0.0;
            }
        } break;

        case CUT_TEA_PARTY: {
            update_main = false;
            game->transition_t += dt;
            draw_tea_party(game, draw_list, audio, dt);
            f32 t = game->transition_t;
            if(t_in_range(t, 5, 10)) {
                draw_simple_text(draw_list, string_const("PROGRAMMER:"), v2_new(4, 56), 0);
                draw_simple_text(draw_list, string_const("CONNER"), v2_new(4, 51), 0);
                draw_simple_text(draw_list, string_const("MOULAISON"), v2_new(4, 46), 0);
            }
            if(t_in_range(t, 12, 17)) {
                draw_simple_text(draw_list, string_const("ART:"), v2_new(4, 56), 0);
                draw_simple_text(draw_list, string_const("HANNAH RANTS"), v2_new(4, 51), 0);
            }
            if(t_in_range(t, 19, 25)) {
                draw_simple_text(draw_list, string_const("MUSIC:"), v2_new(4, 56), 0);
                draw_simple_text(draw_list, string_const("FUNERAL MARCH"), v2_new(4, 51), 0);
                draw_simple_text(draw_list, string_const("OF A MARIONETTE"), v2_new(4, 46), 0);
                draw_simple_text(draw_list, string_const("BY GOUNOD"), v2_new(4, 41), 0);
            }

            if(t_in_range(t, 30, 100000)) {
                draw_simple_text(draw_list, string_const("THE END"), v2_new(4, 56), 0);
            }
        } break;

        default: panic();
    }

    if(update_main) {
        update_visual_state(game, draw_list, v2_zero(), dt);
    }
}
