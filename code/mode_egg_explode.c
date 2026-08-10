#define EGG_COLLECT_TIME 2.0
#define EGG_EXPLODE_TIME 1.5

void mode_egg_explode_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
    state->level_egg_t += dt / EGG_EXPLODE_TIME;
    if(state->level_egg_t > 1.0) {
        state->level_egg_broken = true;
        switch(state->level_egg_index) {
            case 0: {
                String seq[4] = {
                    string_const("HI, I'M A DUCK!"),
                    string_const("YOU CAN CALL ME FUCK CHEZ."),
                    string_const("MARCH ME TO DEATH, MY LOVE."),
                    string_const("IT IS TIME!")
                };
                start_msg(game, seq, 4, MODE_EGG_COLLECT, SPRITE_DUCK_PORTRAIT, ROOT_NOTE_DUCK);
                game->transition_t = 0.0;
            } break;
            default: panic();
        }
    }
    update_visual_state(game, draw_list, v2_new(0, 0), dt);
}


void mode_egg_collect_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
    state->level_egg_collected = true;

    update_visual_state(game, draw_list, v2_new(0, 0), dt);
    v2 egg_draw_pos = v2_scale(v2_from_iv2(state->level_egg_pos), 8.0);
    draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 6, egg_draw_pos, 0);

    i64 duck_spr = SPRITE_DUCK_RIGHT;
    if(state->hannah.pos_cur.x < state->level_egg_pos.x) {
        duck_spr = SPRITE_DUCK_LEFT;
    }
    Entity* last_duck = &state->marchers[state->marchers_len - 1];
    v2 draw_pos = v2_lerp(egg_draw_pos, v2_scale(v2_from_iv2(last_duck->pos_prev), 8.0), game->transition_t);
    f32 sin_t = sin(game->transition_t * M_PI);
    draw_pos.y += sin_t * 12.0;
    draw_sprite(draw_list, duck_spr, 0, draw_pos, 0);

    AudioWaveChannel* wave = &audio->wave_channels[3];
    if(game->transition_t < 0.66) {
        wave->amp = 0.1 + sin_t * 0.3;
        wave->freq = 300.0 + game->transition_t * 1000.0 + sin_t * 100.0;
    } else {
        wave->amp = 0.0;
    }

    game->transition_t += dt / EGG_COLLECT_TIME;
    if(game->transition_t > 1.0) {
        Entity* duck = &state->marchers[state->marchers_len];
        entity_place(duck, last_duck->pos_prev);
        duck->sprite_handle = duck_spr;
        state->marchers_len++;
        game->mode = MODE_GAME;
        game->transition_t = 0.0;
    }

}
