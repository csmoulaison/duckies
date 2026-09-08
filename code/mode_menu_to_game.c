// RELEASE: set these to longer
#define PREMENU_TO_MENU_TIME 5.0

#define MENU_TO_GAME_TIME 6.0

#define LEVEL_SWITCH_TIME 0.2

void mode_level_switch_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
    update_input_move(game);

    game->transition_t += dt / LEVEL_SWITCH_TIME;
    f32 t = game->transition_t;
    if(t > 1.0) {
        t = 1.0;
    } else if(t < 0.0) {
        t = 0.0;
    }
    t = smoothstep(t);
    v2 offset_cur = v2_lerp(v2_scale(state->level_prev_offset_pos, -1.0), v2_zero(), t);
    v2 offset_prev = v2_lerp(v2_zero(), state->level_prev_offset_pos, t);

    draw_level_tiles(game, prev_game_level(game), draw_list, offset_prev);
    update_visual_state(game, draw_list, offset_cur, dt);
    if(game->transition_t > 1.0) {
        Entity* hannah = &state->marchers[0];
        iv2 hannah_delta = iv2_sub(hannah->pos_prev, hannah->pos_cur);
        for(i32 i = 0; i < ducks_len(state); i++) {
            Entity* duck = &state->ducks[i];
            duck->pos_cur = iv2_add(hannah->pos_cur, hannah_delta);
            duck->pos_prev = iv2_add(duck->pos_cur, hannah_delta);
            duck->pos_lead = duck->pos_prev;
            duck->pos_visible = v2_from_iv2(duck->pos_cur);
            duck->pos_prev_visible = v2_from_iv2(duck->pos_prev);
        }
        
        game->mode = MODE_GAME;
        game->transition_t = 0.0;
    	game->saved_state = game->state;
    }
}

void mode_premenu_to_menu_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    String ss[4] = {
        string_const("HANNAH"),
        string_const("AND"),
        string_const("CONNER"),
        string_const("GAME BY"),
    };

    if(game->transition_t > 0.1666 && game->transition_t < 0.8333) {
        for(i32 si = 0; si < 4; si++) {
            i32 ti = (i32)(game->time * 1.0);
            String s = ss[si];
            i32 offx = 10.0;
            i32 offy = 14.0 + si * 5.0;
            i32 font_sz = 4.0;
            i32 pl = 1;
            for(i32 i = 0; i < s.len; i++) {
                draw_sprite(draw_list, SPRITE_FONT_SMALL, (i32)s.text[i] - 32, v2_new(offx + (i * font_sz), offy /* + ysin */), pl);
            }
        }
    }

    game->transition_t += dt / PREMENU_TO_MENU_TIME;
    if(game->transition_t > 1.0) {
        game->mode = MODE_MENU;
        game->transition_t = 0.0;
    }
}

void mode_gate_to_tea_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    game->music_override_state = MUSIC_OVERRIDE_MENU_FADE;
    game->transition_t += dt / 6.0;
    override_pallete_from_fade_t_parameters(draw_list, game->transition_t, 0.30, 0.33, 0.96, 0.99);
    if(game->transition_t > 0.5) {
        game->state.level_index = 49;
        draw_tea_party(game, draw_list, audio, dt);
    } else {
        update_visual_state(game, draw_list, v2_zero(), dt);
    }
    if(game->transition_t > 1.0) {
        game->mode = MODE_CUTSCENE;
        game->cutscene_mode = CUT_TEA_PARTY;
        game->transition_t = 0.0;
    }
}

void mode_menu_to_game_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    game->music_override_state = MUSIC_OVERRIDE_MENU_FADE;
    game->transition_t += dt / MENU_TO_GAME_TIME;
    override_pallete_from_fade_t_parameters(draw_list, game->transition_t, 0.30, 0.33, 0.96, 0.99);

    if(game->transition_t > 0.6) {
        draw_opening(game, draw_list, audio, 0.0);
        
        if(game->transition_t > 1.0) {
            game->mode = MODE_CUTSCENE;
            game->cutscene_mode = CUT_OPENING;
            game->transition_t = 0.0;
        }
    } else {
        draw_main_menu(game, draw_list, dt, game->transition_t);
    }
}
