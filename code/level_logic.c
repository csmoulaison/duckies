#include "level_logic_common.c"

void update_boat_ride(Game* game) {
    LevelState* state = &game->state;
    for(i32 i = 0; i < 5; i++) {
        Entity* platform = push_platform(state);
    }
    if(game->new_cycle_this_frame) {
        if(iv2_eq(state->marchers[0].pos_cur, state->platforms[0].pos_cur)) {
            if(state->level_index == 14 && state->platforms[0].pos_cur.x < 7) {
                if(state->boat_riding) {
                    state->boat_riding = false;
                    for(i32 i = 0; i < 5; i++) {
                        state->platforms[i].move_this_cycle = MOVE_NONE;
                        state->input_move = MOVE_NONE;
                    }
                }
                return;
            }
            state->boat_riding = true;
            for(i32 i = 0; i < 5; i++) {
                entity_move(&state->platforms[i], MOVE_LEFT);
            }
        }
    }
}

void pre_update_level_logic(Game* game, Stack* stack, f32 dt) {
    LevelState* state = &game->state;

    state->egg_exists = false;

    // VOLATILE: For entity types, these need reset
    if(game->new_cycle_this_frame) {
        for(i32 i = 0; i < state->platforms_len; i++) {
            state->platforms[i].pos_prev_visible = v2_from_iv2(state->platforms[i].pos_cur);
        }
        for(i32 i = 0; i < state->cars_len; i++) {
            state->cars[i].pos_prev_visible = v2_from_iv2(state->cars[i].pos_cur);
        }
        state->frogo.pos_prev_visible = v2_from_iv2(state->frogo.pos_cur);
    }
    if(game->half_cycle_this_frame) {
        state->frogo.pos_prev_visible = v2_from_iv2(state->frogo.pos_cur);
    }

    // VOLATILE: These need reset at beginning of thing
    state->platforms_len = 0;
    for(i32 i = 0; i < PLATFORMS_MAX; i++) {
        Entity* platform = &state->platforms[i];
    }
    state->cars_len = 0;
    for(i32 i = 0; i < CARS_MAX; i++) {
        Entity* car = &state->cars[i];
    }
    state->buttons_len = 0;
    for(i32 i = 0; i < BUTTONS_MAX; i++) {
        Button* button = &state->buttons[i];
    }
    state->gates_len = 0;
    for(i32 i = 0; i < GATES_MAX; i++) {
        Gate* gate = &state->gates[i];
    }
    state->signs_len = 0;
    for(i32 i = 0; i < SIGNS_MAX; i++) {
        Sign* sign = &state->signs[i];
    }
    state->queued_time_event.type = TIME_EVENT_NONE;

    state->frogo_exists = false;

    switch(state->level_index) {
        // STARTING AREA:

        // Mult plats
        case 0: { 
            i32 len = platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 4, 2, 2, 0, stack);
                      platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 3, 2, 2, 2, stack);
        } break;

        // Sink cycle
        case 7: { 
            PlatformSinkState cycle[6] = { 
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_WARN,
                PLATFORM_SINK,

                PLATFORM_SINK,
                PLATFORM_WARN
            };
            // This for loop could have a set_pos_rect helper and just be an i
            for(i32 x = 0; x < 2; x++) {
                for(i32 y = 0; y < 3; y++) {
                    Entity* plat = push_platform(state);
                    platform_sink_cycle(game, plat, cycle, 6, 2 + 3 - y);
                    platform_set_pos(plat, iv2_new(3 + x, 2 + y));
                }
            }
        } break;

        // Snake test
        case 2: { 
            iv2 positions[16] = {
                {{{ -3, 3 }}},
                {{{ -2, 3 }}},
                {{{ -1, 3 }}},
                {{{ 0, 3 }}},

                {{{ 1, 3 }}},
                {{{ 2, 3 }}},
                {{{ 3, 3 }}},
                {{{ 4, 3 }}},

                {{{ 5, 3 }}},
                {{{ 5, 4 }}},
                {{{ 5, 5 }}},
                {{{ 5, 6 }}},

                {{{ 5, 7 }}},
                {{{ 5, 8 }}},
                {{{ 5, 9 }}},
                {{{ 5, 10 }}}
            };
            PlatformSinkState sinks[16] = { 
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,

                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,

                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_WARN,

                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK
            };
            for(i32 i = 0; i < 3; i++) {
                Entity* platform = push_platform(state);
                platform_follow_sequence(game, platform, positions, 16, i);
                platform_sink_cycle(game, platform, sinks, 16, i);
            }
        } break;

        // Egg 1
        case 3: {
            place_egg(state, iv2_new(1, 2), 0);
        } break;

        // First water boat
        case 9: {
            i32 len = platform_push_run_x_back_forth_sequence(game, 2, 2, 4, 3, 2, 2, 0, stack);
        } break;

        // Egg 2
        case 10: {
            place_egg(state, iv2_new(4, 2), 1);
            place_button(state, iv2_new(3, 5));
            place_button(state, iv2_new(4, 5));
            place_button(state, iv2_new(4, 4));
            i32 trigger_buttons[3] = { 0, 1, 2 };
            place_button_gate(state, iv2_new(3, 7), trigger_buttons, 3, 0);
            place_button_gate(state, iv2_new(4, 7), trigger_buttons, 3, 0);
        } break;

        // Frog 1
        case 11: { 
            place_sign(state, iv2_new(1, 2), SIGN_FROG_1, SPRITE_FROG);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_B, 2, 0), 0, 4, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_B, 2, 1), 3, 4, 12);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_TRUCK, 2, 0), 3, 5, 12);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_B, 2, 2), 6, 5, 12);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_A, 2, 0), 9, 5, 12);
        } break;

        // Frog 2
        case 12: {
            place_sign(state, iv2_new(0, 0), SIGN_FROG_2, SPRITE_FROG);

            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 2), 6, 1, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_TRUCK, 2, 1), 9, 1, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_B, 2, 2), 12, 1, 12);

            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_B, 2, 1), 4, 2, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 1), 7, 2, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_B, 2, 0), 10, 2, 12);

            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_B, 2, 0), 4, 3, 16);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_B, 2, 1), 7, 4, 16);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_TRUCK, 2, 0), 10, 4, 16);
        } break;

        // Frog 3
        case 13: {
            place_sign(state, iv2_new(2, 0), SIGN_FROG_3, SPRITE_FROG);

            // road 1
            platform_car_cycle_rtl(game, push_car(state, SPRITE_TRUCK, 2, 2), 6,  1, 16);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_B, 2, 1), 9,  1, 16);

            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_B, 2, 0), 0,  2, 16);

            // road 2
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_B, 2, 0), 0,  4, 16);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 1), 3,  4, 16);

            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_B, 2, 2), 0,  5, 16);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_B, 2, 1), 9,  5, 16);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_BUS, 3, 0), 4,  5, 16);


            // game count x1 x2 y perendcount perstepcount indexoff stack
            platform_push_run_x_back_forth_sequence(game, 4, 0, 4, 3, 2, 2, 0, stack);
            platform_push_run_x_back_forth_sequence(game, 4, 0, 4, 6, 2, 2, 8, stack);
        } break;

        // Frog end and sign post
        case 14: {
            place_sign(state, iv2_new(3, 4), SIGN_POST_1, SPRITE_SIGN);
            place_sign(state, iv2_new(4, 4), SIGN_FROG_4, SPRITE_FROG);

            if(state->boat_activated) {
                update_boat_ride(game);
            }
        } break;

        // SWAMP AREA

        // T River
        case 50: {
            if(!state->boat_activated) {
                platform_push_run_x_back_forth_sequence(game, 3, 1, 5, 5, 2, 2, 0, stack);
                platform_push_run_x_back_forth_sequence(game, 4, 0, 4, 4, 2, 2, 0, stack);
                platform_push_run_x_back_forth_sequence(game, 3, 3, 5, 3, 2, 2, 2, stack);
            }

            if(state->boat_riding) {
                update_boat_ride(game);
            }
        } break;

        // Button sidebar
        // ACTIVATES PERMAGATE 1 AND TIMED EVENT
        case 34: {
            place_button(state, iv2_new(5, 5));
            place_button(state, iv2_new(5, 6));
            place_button(state, iv2_new(6, 5));
            place_button(state, iv2_new(6, 6));
            i32 buttons[4] = { 0, 1, 2, 3 };
            place_button_gate(state, iv2_new(5, 4), buttons, 4, 1);
            place_button_gate(state, iv2_new(6, 4), buttons, 4, 1);
            //place_button_gate_time_event(state, buttons, 4, TIME_EVENT_BUTTON_GATE, 1, 32);
        } break;

        // River with timed gate
        // PERMAGATE 1 ACTIVATED FROM LEVEL 34
        case 51: {
            place_remote_gate(state, iv2_new(5, 0), 1);

            if(state->boat_riding) {
                update_boat_ride(game);
            }
        } break;

        // Memory pads
        case 61: {
            iv2 positions[16] = {
                {{{ 5, 6 }}},
                {{{ 5, 5 }}},
                {{{ 4, 5 }}},
                {{{ 3, 5 }}},

                {{{ 2, 5 }}},
                {{{ 2, 4 }}},
                {{{ 2, 3 }}},
                {{{ 3, 3 }}},

                {{{ 4, 3 }}},
                {{{ 5, 3 }}},
                {{{ 5, 2 }}},
                {{{ 4, 2 }}},

                {{{ 4, 1 }}},
                {{{ 5, 1 }}},
                {{{ 6, 1 }}},
                {{{ 6, 2 }}}
            };
            PlatformSinkState sinks[16] = { 
                PLATFORM_FLOAT,
                PLATFORM_WARN,
                PLATFORM_WARN,
                PLATFORM_WARN,

                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,

                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,

                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK
            };
            for(i32 i = 0; i < 16; i++) {
                Entity* p = push_platform(state);
                platform_set_pos(p, positions[15 - i]);
                platform_sink_cycle(game, p, sinks, 16, i);
            }
        } break;

        // On the way to egg 3, first crumblers
        case 52: {
            iv2 crumb_pos[12] = {
                iv2_new(1, 2),
                iv2_new(1, 3),
                iv2_new(2, 1),
                iv2_new(2, 2),

                iv2_new(3, 2),
                iv2_new(3, 3),
                iv2_new(4, 1),
                iv2_new(4, 2),

                iv2_new(5, 2),
                iv2_new(5, 3),
                iv2_new(6, 1),
                iv2_new(6, 2)
            };
            for(i32 i = 0; i < 12; i++) {
                place_crumbler(state, push_platform_and_pos(state, crumb_pos[i]), 4);
            }
            PlatformSinkState sinks[10] = {
                PLATFORM_WARN,
                PLATFORM_FLOAT,
                PLATFORM_WARN,
                PLATFORM_SINK,

                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_SINK,

                PLATFORM_SINK,
                PLATFORM_SINK
            };
            for(i32 i = 0; i < 6; i++) {
                platform_sink_cycle(game, push_platform_and_pos(state, iv2_new(i + 1, 4)), sinks, 10, i);
            }

        } break;

        // Ruins crossroads
        case 57: {
            platform_push_run_x_back_forth_sequence(game, 2, 1, 5, 5, 2, 1, 0, stack);
            platform_push_run_x_back_forth_sequence(game, 2, 1, 5, 2, 2, 1, 5, stack);
            PlatformSinkState sinks[8] = { 
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_WARN,
                PLATFORM_SINK,

                PLATFORM_WARN,
                PLATFORM_FLOAT,
                PLATFORM_FLOAT,
                PLATFORM_FLOAT
            };
            platform_sink_cycle(game, push_platform_and_pos(state, iv2_new(1, 3)), sinks, 8, 0);
            platform_sink_cycle(game, push_platform_and_pos(state, iv2_new(6, 4)), sinks, 8, 4);
        } break;

        // Hilbert's Lake
        case 66: {
            {
                iv2 ps[4] = {
                    iv2_new(5, 2),
                    iv2_new(6, 2),
                    iv2_new(6, 3),
                    iv2_new(5, 3)
                };
                for(i32 i = 0; i < 3; i++) {
                    platform_follow_sequence(game, push_platform(state), ps, 4, i);
                }
            }
            {
                iv2 ps[4] = {
                    iv2_new(4, 5),
                    iv2_new(3, 5),
                    iv2_new(3, 4),
                    iv2_new(4, 4)
                };
                for(i32 i = 0; i < 3; i++) {
                    platform_follow_sequence(game, push_platform(state), ps, 4, i);
                }
            }
            {
                iv2 ps[4] = {
                    iv2_new(2, 3),
                    iv2_new(2, 2),
                    iv2_new(1, 2),
                    iv2_new(1, 3)
                };
                for(i32 i = 0; i < 3; i++) {
                    platform_follow_sequence(game, push_platform(state), ps, 4, i);
                }
            }
            {
                iv2 ps[8] = {
                    iv2_new(1, 4),
                    iv2_new(1, 5),
                    iv2_new(2, 4),
                    iv2_new(2, 5),

                    iv2_new(5, 4),
                    iv2_new(5, 5),
                    iv2_new(6, 4),
                    iv2_new(6, 5)
                };
                for(i32 i = 0; i < 8; i++) {
                    place_crumbler(state, push_platform_and_pos(state, ps[i]), 4);
                }
            }
            {
                iv2 ps[8] = {
                    iv2_new(5, 5),
                    iv2_new(5, 5),
                    iv2_new(5, 5),
                    iv2_new(5, 5),

                    iv2_new(5, 6),
                    iv2_new(5, 6),
                    iv2_new(5, 6),
                    iv2_new(5, 6)
                };
                //platform_follow_sequence(game, push_platform(state), ps, 4, 0);
            }
        } break;

        // Egg 3
        case 37: {
            place_egg(state, iv2_new(4, 2), 2);
        } break;

        // Muffin intro
        case 58: {
            place_sign(state, iv2_new(4, 4), SIGN_MUFFIN_1, SPRITE_MUFFIN_LEFT);
        } break;

        // Egg 4
        case 68: {
            place_egg(state, iv2_new(4, 4), 3);
            place_sign(state, iv2_new(4, 3), SIGN_MUFFIN_2, SPRITE_MUFFIN_LEFT);
        } break;

        // Boat start
        case 64: {
            if(!state->boat_activated) {
                state->boat_activated = true;
                for(i32 i = 0; i < 5; i++) {
                    Entity* platform = push_platform_and_pos(state, iv2_new(5 + i, 4));
                }
            }
            update_boat_ride(game);
        } break;

        // river boat 2
        case 65: {
            if(state->boat_riding) {
                update_boat_ride(game);
            }
        } break;

        // river boat 3
        case 60: {
            if(state->boat_riding) {
                update_boat_ride(game);
            }
        } break;

        // Outside Frogo
        case 16: {
            place_button(state, iv2_new(1, 6));
            place_button(state, iv2_new(2, 6));
            place_button(state, iv2_new(3, 6));
            place_button(state, iv2_new(4, 6));
            place_button(state, iv2_new(5, 6));
            i32 buttons[6] = { 0, 1, 2, 3, 4 };
            place_button_gate(state, iv2_new(6, 7), buttons, 5, 3);
        } break;

        // Frogo 1
        case 17: {
            iv2 p[20] = {
                iv2_new(1, 1),
                iv2_new(1, 2),
                iv2_new(1, 3),
                iv2_new(1, 4),
                iv2_new(1, 5),
                iv2_new(1, 6),

                iv2_new(1, 6),
                iv2_new(2, 6),
                iv2_new(3, 6),
                iv2_new(4, 6),

                iv2_new(4, 6),
                iv2_new(4, 5),
                iv2_new(4, 4),
                iv2_new(4, 3),
                iv2_new(4, 2),
                iv2_new(4, 1),

                iv2_new(4, 1),
                iv2_new(3, 1),
                iv2_new(2, 1),
                iv2_new(1, 1),
            };
            if(!state->frogo_appeased) place_frogo(game, p, 20);
        } break;

        // Frogo 2
        case 18: {
            iv2 p[12] = {
                iv2_new(6, 4),
                iv2_new(5, 4),
                iv2_new(4, 4),
                
                iv2_new(4, 4),
                iv2_new(4, 3),
                iv2_new(4, 2),

                iv2_new(4, 2),
                iv2_new(5, 2),
                iv2_new(6, 2),

                iv2_new(6, 2),
                iv2_new(6, 3),
                iv2_new(6, 4),
            };
            if(!state->frogo_appeased) place_frogo(game, p, 12);
        } break;

        // Frogo 3
        case 19: {
            iv2 p[12] = {
                iv2_new(2, 4),
                iv2_new(3, 4),
                iv2_new(4, 4),

                iv2_new(4, 3),
                iv2_new(4, 2),
                iv2_new(4, 1),

                iv2_new(3, 1),
                iv2_new(2, 1),
                iv2_new(1, 1),

                iv2_new(1, 2),
                iv2_new(1, 3),

                iv2_new(2, 3),
            };
            if(!state->frogo_appeased) place_frogo(game, p, 12);
        } break;

        // Frogo 4
        case 20: {
            place_button(state, iv2_new(1, 6));
            place_button(state, iv2_new(4, 6));
            place_button(state, iv2_new(2, 3));
            i32 buttons[3] = { 0, 1, 2 };
            place_button_gate(state, iv2_new(6, 7), buttons, 3, 4);

            iv2 p[13] = {
                iv2_new(1, 6),
                iv2_new(2, 6),
                iv2_new(3, 6),
                iv2_new(4, 6),

                iv2_new(4, 5),
                iv2_new(4, 4),
                iv2_new(4, 3),

                iv2_new(3, 3),
                iv2_new(2, 3),
                iv2_new(1, 3),

                iv2_new(1, 4),
                iv2_new(1, 5),
            };
            if(!state->frogo_appeased) place_frogo(game, p, 12);
        } break;

        // Final frogo
        case 21: {
            place_sign(state, iv2_new(2, 3), SIGN_FROGO, SPRITE_FROG_LEAP_RIGHT);
            state->frogo_appeased = true;
        } break;

        // Troll
        case 22: {
            iv2 pos = iv2_new(3, 5);
            if(state->troll_appeased) pos = iv2_new(1, 5);
            Sign* sign = place_sign(state, pos, SIGN_TROLL_1, SPRITE_TROLL);
            sign->doublewide = true;
        } break;

        // Egg 5
        case 28: {
            place_egg(state, iv2_new(2, 2), 4);
        } break;

        // Large gate
        case 35: {
            place_button(state, iv2_new(1, 2));
            place_button(state, iv2_new(2, 2));
            place_button(state, iv2_new(3, 2));
            place_button(state, iv2_new(4, 2));
            place_button(state, iv2_new(5, 2));
            place_button(state, iv2_new(6, 2));
            i32 buttons[6] = { 0, 1, 2, 3, 4, 5 };
            place_button_gate(state, iv2_new(-1, -1), buttons, 5, 2);
        } break;

        // Snake minigame
        case 36: {
            // tiles must come first for correct indices.
            for(i32 i = 0; i < 36; i++) {
                push_platform_and_pos(state, iv2_new(1 + i % 6, 1 + i / 6));
            }

            iv2 positions[28] = {
                iv2_new(0, 0),
                iv2_new(1, 0),
                iv2_new(2, 0),
                iv2_new(3, 0),
                iv2_new(4, 0),
                iv2_new(5, 0),
                iv2_new(6, 0),
                iv2_new(7, 0),

                iv2_new(7, 1),
                iv2_new(7, 2),
                iv2_new(7, 3),
                iv2_new(7, 4),
                iv2_new(7, 5),
                iv2_new(7, 6),

                iv2_new(7, 7),
                iv2_new(6, 7),
                iv2_new(5, 7),
                iv2_new(4, 7),
                iv2_new(3, 7),
                iv2_new(2, 7),
                iv2_new(1, 7),
                iv2_new(0, 7),

                iv2_new(0, 6),
                iv2_new(0, 5),
                iv2_new(0, 4),
                iv2_new(0, 3),
                iv2_new(0, 2),
                iv2_new(0, 1),
            };
            for(i32 i = 0; i < 8; i++) {
                Entity* segment = push_platform(state);
                platform_follow_sequence(game, segment, positions, 28, i);
                segment->snake_index = i + 1;
            }
        } break;

        default: break;
    } 
}

void post_update_level_logic(Game* game) {
    LevelState* state = &game->state;
    switch(state->level_index) {
        default: break;
    }
}
