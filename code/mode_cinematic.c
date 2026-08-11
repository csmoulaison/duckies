void start_cutscene(Game* game, CutsceneMode mode) {
    game->transition_t = 0.0;
    game->cutscene_mode = mode;
    game->mode = MODE_CUTSCENE;
}

void mode_cutscene_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    String msg[16];
    switch(game->cutscene_mode) {
        case CUT_FROG_PAUSE: {
            game->transition_t += dt / 2.0;
            if(game->transition_t > 1.0) {
                String msg[3] = {
                    string_const("TRY CROSSING THIS ROAD."),
                    string_const("IT'S A REAL THRILL, AS I'M SURE YOU'LL FIND."),
                    string_const("WE'LL MEET UP AHEAD.")
                };
                start_msg(game, msg, 3, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
            }
        } break;

        case CUT_FROG_INVITATION: {
            printf("waiting for invite\n");
            game->transition_t += dt / 0.75;
            if(game->transition_t > 1.0) {
                printf("inviting!\n");
                String msg[4] = {
                    string_const("HEY, MISTER FROG!"),
                    string_const("WE'RE ON OUR WAY TO A TEA PARTY, YOU SHOULD COME ALONG!"),
                    string_const("THERE WILL BE TEA... AND..."),
                    string_const("AND ALL THE OTHER NECESSARY TEA PARTY ACCOUTREM- ENTS!")
                };
                start_msg_queue_cutscene(game, msg, 4, SPRITE_DUCK_PORTRAIT, MSG_SOUND_DUCK, CUT_FROG_INVITATION_2);
            }
        } break;

        case CUT_FROG_INVITATION_2: {
            String msg[3] = {
                string_const("TEA PARTY, YOU SAY?"),
                string_const("RIBBIT."),
                string_const("I'LL BE THERE. I'LL SEE YOU AT TEA TIME.")
            };
            start_msg(game, msg, 3, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
        } break;

        default: panic();
    }

    update_visual_state(game, draw_list, v2_zero(), dt);
}
