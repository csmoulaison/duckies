#define MSG_CHAR_TIME 0.1

f32 random_msg_note(f32 root_note) {
    return root_note * 1.5f * random_f32();
}

void start_msg(Game* game, String* strings, i32 len, GameMode queued_game_mode, i64 portrait, f32 root_note) {
    assert(len <= MSG_LEN_MAX);
    for(i32 i = 0; i < len; i++) {
        game->msg[i] = strings[i];
    }
    game->msg_len = len;
    game->msg_string_cur = 0;
    game->msg_char_cur = 0;
    game->msg_char_t = 0.0;
    game->queued_game_mode = queued_game_mode;
    game->msg_speeding = false;
    game->mode = MODE_MSG;
    game->msg_sound_root_note = root_note;
    game->msg_sound_char_note = random_msg_note(root_note);
    game->msg_portrait = portrait;
}

void start_msg_queue_cutscene(Game* game, String* strings, i32 len, GameMode queued_game_mode, i64 portrait, f32 root_note, CutsceneMode cutscene) {
    start_msg(game, strings, len, MODE_CUTSCENE, portrait, root_note);
    game->cutscene_mode = CUT_FROG_PAUSE;
}

void mode_msg_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt, Stack* stack) {
    String s = game->msg[game->msg_string_cur];
    //String test = string_from_stack(stack, s.len + 1);
    //string_cat(&test, s);
    //string_write_null_terminator(&test);
    //printf("string %s\n", test.text);

    // Update
    bool speed_this_frame = false;
    if(game->msg_char_cur < s.len) {
        if(game->msg_speeding) {
            game->msg_char_t += dt / (MSG_CHAR_TIME / 16.0);
        } else {
            game->msg_char_t += dt / MSG_CHAR_TIME;
        }
        if(game->msg_char_t > 1.0) {
            game->msg_char_cur++;
            game->msg_char_t = 0.0;
            game->msg_sound_char_note = random_msg_note(game->msg_sound_root_note);
        }
    	if(input_button_pressed(game->input_buttons[BUTTON_START])) {
        	game->msg_speeding = true;
        	speed_this_frame = true;
    	}
    	AudioWaveChannel* wave = &audio->wave_channels[3];
    	wave->amp = game->msg_char_t * 0.5;
    	wave->freq = game->msg_sound_root_note + game->msg_sound_char_note + game->msg_char_t * 400.0;
    }

    if((!speed_this_frame && game->msg_speeding) || game->msg_char_cur >= s.len) {
        if(input_button_pressed(game->input_buttons[BUTTON_START])) {
        	game->msg_string_cur++;
        	if(game->msg_string_cur >= game->msg_len) {
                game->mode = game->queued_game_mode;
                game->transition_t = 0.0;
        	} else {
            	game->msg_char_cur = 0;
            	game->msg_speeding = false;
        	}
        }
	}

    // Draw
    update_visual_state(game, draw_list, v2_zero(), dt);
    //for(i32 i = 0; i < 4; i++) {
    //    i32 x = i % 2;
    //    i32 y = i / 2;
        //draw_sprite_layer(draw_list, game->msg_portrait, i, v2_new(x * 8.0, 8.0 - y * 8.0), 0, 1);
        draw_sprite_layer(draw_list, game->msg_portrait, 0, v2_new(0.0, 0.0), 0, 1);
    //}
    for(i32 i = 0; i < 6; i++) {
        draw_sprite_layer(draw_list, SPRITE_DIALOGUE_BOX, 1, v2_new(16.0 + i * 8.0, 0.0), 0, 1);
        draw_sprite_layer(draw_list, SPRITE_DIALOGUE_BOX, 0, v2_new(16.0 + i * 8.0, 8.0), 0, 1);
    }

    v2* places = text_placements(s, v2_new(4, 5), v4_new(17, 1, 47, 15), false, stack);
    i32 yoff = 0;
    for(i32 i = 0; i < game->msg_char_cur; i++) {
        if(places[i].y <= -yoff) {
            yoff += 1;
        }
    }
    for(i32 i = 0; i < game->msg_char_cur; i++) {
        v2 pos = v2_new(places[i].x, places[i].y + yoff);
        if(pos.y > 16 - 5) continue;

        i32 palette = 1;
        if(game->msg_char_cur != s.len) {
            i32 end_delta = game->msg_char_cur - i - 1;
            if(end_delta < 2) {
                pos.y -= end_delta;
                palette = end_delta;
            }
        }

        draw_sprite_layer(draw_list, SPRITE_FONT_SMALL, (i32)s.text[i] - 32, pos, palette, 1);
    }
}
