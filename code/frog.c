typedef enum {
    SIGN_FROG_1,
    SIGN_FROG_2,
    SIGN_FROG_3,
    SIGN_FROG_4,
    SIGN_POST_1,
    SIGN_MUFFIN_1,
    SIGN_MUFFIN_2,
    SIGN_TROLL_1,
    SIGN_FROGO
} SignType;

void sign_talk(Game* game, Sign* sign) {
    LevelState* state = &game->state;
    switch(sign->index) {
        case SIGN_FROG_1: {
            if(sign->talk_count == 0) {
                Msg msg[5] = {
                    new_msg(MSG_FROG, string_const("HOWDY, STRANGER.")),
                    new_msg(MSG_FROG, string_const("AS YOU CAN SEE, IM A FROG.")),
                    new_msg(MSG_FROG, string_const("I'D LIKE TO BE A NICE GREEN COLOR.")),
                    new_msg(MSG_FROG, string_const("A NICE TREE FROG GREEN.")),
                    new_msg(MSG_FROG, string_const("BUT YOU WOULDN'T BE ABLE TO SEE ME..."))
                };
                start_msg_queue_cutscene(game, msg, 5, CUT_FROG_PAUSE);
            } else {
                Msg msg[1] = {
                    new_msg(MSG_FROG, string_const("RIBBIT..."))
                };
                start_msg(game, msg, 1, MODE_GAME);
            }
        } break;

        case SIGN_FROG_2: {
            if(sign->talk_count == 0) {
                Msg msg[2] = {
                    new_msg(MSG_FROG, string_const("I'VE GOT A HOT TIP FOR YOU.")),
                    new_msg(MSG_FROG, string_const("WHEN CROSSING THE ROAD, REMEBER TO WATCH OUT FOR CARS."))
                };
                start_msg(game, msg, 2, MODE_GAME);
            } else {
                Msg msg[1] = {
                    new_msg(MSG_FROG, string_const("DON'T SAY I NEVER DID ANYTHING FOR YOU."))
                };
                start_msg(game, msg, 1, MODE_GAME);
            }
        } break;

        case SIGN_FROG_3: {
            if(sign->talk_count == 0) {
                Msg msg[3] = {
                    new_msg(MSG_FROG, string_const("LOTS OF HIGHWAYS AROUND HERE.")),
                    new_msg(MSG_FROG, string_const("SO MUCH NOISE. SO MUCH DANGER.")),
                    new_msg(MSG_FROG, string_const("ALL SO THEY CAN PUT UP ANOTHER DATA CENTER."))
                };
                start_msg(game, msg, 3, MODE_GAME);
            } else {
                Msg msg[1] = {
                    new_msg(MSG_FROG, string_const("TELL ME, WHERE DO THE CHILDREN PLAY?"))
                };
                start_msg(game, msg, 1, MODE_GAME);
            }
        } break;

        case SIGN_FROG_4: {
            if(sign->talk_count == 0) {
                Msg msg[2] = {
                    new_msg(MSG_FROG, string_const("THAT WAS SOME SLICK MARCHING.")),
                    new_msg(MSG_FROG, string_const("IT'S BEEN FUN HITTING THE ROADS WITH YOU."))
                };
                start_msg_queue_cutscene(game, msg, 2, CUT_FROG_INVITATION);
            } else {
                Msg msg[1] = {
                    new_msg(MSG_FROG, string_const("SEE YOU AT THE PARTY!"))
                };
                start_msg(game, msg, 1, MODE_GAME);
            }
        } break;

        case SIGN_POST_1: {
            // #, $, %, &: N, W, S, E arrows
            Msg msg[1] = {
                new_msg(MSG_SIGN, string_const("N# TEA TIME   W$ HIGHLANDS E& OLD RUINS"))
            };
            start_msg(game, msg, 1, MODE_GAME);
        } break;

        case SIGN_MUFFIN_1: {
            state->honking_unlocked = true;
            Msg msg[6] = {
                new_msg(MSG_MUFFIN, string_const("HI, I'M A MUFFIN SPROUT!")),
                new_msg(MSG_MUFFIN, string_const("MY FELLOW SPROUTS ARE HAVING TROUBLE GETTING TO SLEEP.")),
                new_msg(MSG_MUFFIN, string_const("WOULD YOU MIND SINGING THEM A LULLABY?")),
                new_msg(MSG_MUFFIN, string_const("DUCKS HONKING IS MOST RELAXING TO US MUFFIN SPROUTS!")),
                new_msg(MSG_MUFFIN, string_const("TRY PRESSING SPACE TO HONK!")),
                new_msg(MSG_MUFFIN, string_const("ALL THE SPROUT- LINGS ARE DOWN THE PATH."))
            };
            start_msg(game, msg, 6, MODE_GAME);
        } break;

        case SIGN_MUFFIN_2: {
            Msg msg[3] = {
                new_msg(MSG_MUFFIN, string_const("FOR DAYS UNTOLD WE HAVE WORSHIPPED THE ORB.")),
                new_msg(MSG_MUFFIN, string_const("AND NOW IT PASSES TO YOU!")),
                new_msg(MSG_MUFFIN, string_const("SWORN SISTER AND FRIEND FOREVER OF THE MUFFIN SPROUTS."))
            };
            start_msg(game, msg, 3, MODE_GAME);
        } break;

        case SIGN_TROLL_1: {
            Msg msg[18] = {
                new_msg(MSG_TROLL, string_const("GRRR!!!")),
                new_msg(MSG_DUCK, string_const("OH DEAR!")),
                new_msg(MSG_TROLL, string_const("BEWARE MY POWER!!!")),
                new_msg(MSG_HANNAH, string_const("...")),
                new_msg(MSG_TROLL, string_const("...")),
                new_msg(MSG_DUCK, string_const("...")),
                new_msg(MSG_HANNAH, string_const("...")),
                new_msg(MSG_DUCK, string_const("...")),
                new_msg(MSG_TROLL, string_const("...")),
                new_msg(MSG_DUCK, string_const("WELL?")),
                new_msg(MSG_TROLL, string_const("I, UH.")),
                new_msg(MSG_HANNAH, string_const("...")),
                new_msg(MSG_TROLL, string_const("I NEVER WAS VERY GOOD AT THIS.")),
                new_msg(MSG_TROLL, string_const("MAYBE I JUST LET YOU CROSS THE BRIDGE AND WE FORGET ANY OF THIS EVER HAPPENED.")),
                new_msg(MSG_DUCK, string_const("THAT SOUNDS AMENABLE TO ME!!")),
                new_msg(MSG_TROLL, string_const("YEAH OKAY.")),
                new_msg(MSG_DUCK, string_const("OH, MR. TROLL, YOU WOULDN'T FANCY A TEA PARTY?")),
                new_msg(MSG_TROLL, string_const("SIGN ME UP BUTTERCUP."))
            };
            start_msg(game, msg, 18, MODE_APPEASE_TROLL);
        } break;

        case SIGN_FROGO: {
            Msg msg[3] = {
                new_msg(MSG_FROGO, string_const("YOU'VE GOT SKILLS, KID.")),
                new_msg(MSG_FROGO, string_const("I'D BE HONORED TO JOIN YOUR TEA PARTY.")),
                new_msg(MSG_FROGO, string_const("THANKS FOR THE FUN."))
            };
            start_msg(game, msg, 3, MODE_GAME);
        } break;

        default: break;
    }
    sign->talk_count++;
}
