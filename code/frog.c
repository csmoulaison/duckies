void frog_talk(Game* game) {
    LevelState* state = &game->state;
    switch(state->frog_index) {
        case 0: {
            String msg[5] = {
                string_const("HOWDY, STRANGER!"),
                string_const("AS YOU CAN SEE, IM A FROG."),
                string_const("I'D LIKE TO BE A NICE GREEN COLOR."),
                string_const("A NICE TREE FROG GREEN."),
                string_const("BUT YOU WOULDN'T BE ABLE TO SEE ME...")
            };
            start_msg_queue_cutscene(game, msg, 5, MODE_CUTSCENE, SPRITE_FROG_PORTRAIT, ROOT_NOTE_FROG, CUT_FROG_PAUSE);
        } break;
    }
}
