typedef enum {
    SIGN_FROG_1,
    SIGN_FROG_2,
    SIGN_FROG_3,
    SIGN_FROG_4,
    SIGN_POST_1
} SignType;

void sign_talk(Game* game, Sign* sign) {
    LevelState* state = &game->state;
    switch(sign->index) {
        case SIGN_FROG_1: {
            if(sign->talk_count == 0) {
                String msg[5] = {
                    string_const("HOWDY, STRANGER."),
                    string_const("AS YOU CAN SEE, IM A FROG."),
                    string_const("I'D LIKE TO BE A NICE GREEN COLOR."),
                    string_const("A NICE TREE FROG GREEN."),
                    string_const("BUT YOU WOULDN'T BE ABLE TO SEE ME...")
                };
                start_msg_queue_cutscene(game, msg, 5, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG, CUT_FROG_PAUSE);
            } else {
                String msg[1] = {
                    string_const("RIBBIT..."),
                };
                start_msg(game, msg, 1, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
            }
        } break;

        case SIGN_FROG_2: {
            if(sign->talk_count == 0) {
                String msg[2] = {
                    string_const("I'VE GOT A HOT TIP FOR YOU."),
                    string_const("WHEN CROSSING THE ROAD, REMEBER TO WATCH OUT FOR CARS.")
                };
                start_msg(game, msg, 2, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
            } else {
                String msg[1] = {
                    string_const("DON'T SAY I NEVER DID ANYTHING FOR YOU."),
                };
                start_msg(game, msg, 1, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
            }
        } break;

        case SIGN_FROG_3: {
            if(sign->talk_count == 0) {
                String msg[3] = {
                    string_const("LOTS OF HIGHWAYS AROUND HERE."),
                    string_const("SO MUCH NOISE. SO MUCH DANGER."),
                    string_const("ALL SO THEY CAN PUT UP ANOTHER DATA CENTER.")
                };
                start_msg(game, msg, 3, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
            } else {
                String msg[1] = {
                    string_const("TELL ME, WHERE DO THE CHILDREN PLAY?"),
                };
                start_msg(game, msg, 1, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
            }
        } break;

        case SIGN_FROG_4: {
            if(sign->talk_count == 0) {
                String msg[2] = {
                    string_const("THAT WAS SOME SLICK MARCHING."),
                    string_const("IT'S BEEN FUN HITTING THE ROADS WITH YOU.")
                };
                start_msg_queue_cutscene(game, msg, 2, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG, CUT_FROG_INVITATION);
            } else {
                String msg[1] = {
                    string_const("SEE YOU AT THE PARTY!"),
                };
                start_msg(game, msg, 1, MODE_GAME, SPRITE_FROG_PORTRAIT, MSG_SOUND_FROG);
            }
        } break;

        case SIGN_POST_1: {
            // #, $, %, &: N, W, S, E arrows
            String msg[1] = {
                string_const("N# TEA TIME   W$ HIGHLANDS E& SWAMP")
            };
            start_msg(game, msg, 1, MODE_GAME, -1, MSG_SOUND_SIGN);
        } break;
        default: break;
    }
    sign->talk_count++;
}
