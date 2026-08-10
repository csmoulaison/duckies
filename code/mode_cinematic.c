void start_cutscene(Game* game, CutsceneMode mode) {
    game->transition_t = 0.0;
    game->cutscene_mode = mode;
    game->mode = MODE_CUTSCENE;
}

void mode_cutscene_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    switch(game->cutscene_mode) {
        case CUT_FROG_PAUSE: {
            game->transition_t += dt / 2.0;
            if(game->transition_t > 1.0) {
                String msg[4] = {
                    string_const("ANYHOO..."),
                    string_const("TRY CROSSING THIS ROAD."),
                    string_const("IT'S A REAL THRILL, AS I'M SURE YOU'LL FIND."),
                    string_const("WE'LL MEET UP AHEAD.")
                };
                start_msg(game, msg, 4, MODE_GAME, SPRITE_FROG_PORTRAIT, ROOT_NOTE_FROG);
            }
        } break;
        default: panic();
    }

    update_visual_state(game, draw_list, v2_zero(), dt);
}
