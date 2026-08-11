#include "level_logic_common.c"

void pre_update_level_logic(Game* game, Stack* stack) {
    LevelState* state = &game->state;

    state->egg_exists = false;

    if(game->new_cycle_this_frame) {
        for(i32 i = 0; i < state->platforms_len; i++) {
            state->platforms[i].pos_prev_visible = v2_from_iv2(state->platforms[i].pos_cur);
        }
        for(i32 i = 0; i < state->cars_len; i++) {
            state->cars[i].pos_prev_visible = v2_from_iv2(state->cars[i].pos_cur);
        }
    }

    state->platforms_len = 0;
    for(i32 i = 0; i < PLATFORMS_MAX; i++) {
        Entity* platform = &state->platforms[i];
        platform->sink_state = PLATFORM_FLOAT;
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

    switch(state->level_index) {
        // mult plats
        case 0: { 
            i32 len = platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 4, 2, 2, 0, stack);
                      platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 3, 2, 2, 2, stack);
        } break;

        // sink cycvle
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
                    platform_set_pos(game, plat, iv2_new(3 + x, 2 + y));
                }
            }
        } break;

        // snake test
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
            place_egg(state, iv2_new(2, 2), 0);
        } break;

        // first water boat
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

        // frog 1
        case 11: { 
            place_sign(state, iv2_new(1, 2), SIGN_FROG_1, SPRITE_FROG);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 0), 0, 4, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 1), 3, 4, 12);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_A, 2, 0), 3, 5, 12);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_A, 2, 2), 6, 5, 12);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_A, 2, 0), 9, 5, 12);
        } break;

        // frog 2
        case 12: {
            place_sign(state, iv2_new(0, 0), SIGN_FROG_2, SPRITE_FROG);

            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 2), 6, 1, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 1), 9, 1, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 2), 12, 1, 12);

            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 1), 4, 2, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 1), 7, 2, 12);
            platform_car_cycle_rtl(game, push_car(state, SPRITE_CAR_A, 2, 0), 10, 2, 12);

            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_A, 2, 0), 4, 3, 16);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_A, 2, 1), 7, 4, 16);
            platform_car_cycle_ltr(game, push_car(state, SPRITE_CAR_A, 2, 0), 10, 4, 16);
        } break;

        // frog 3
        case 13: {
            place_sign(state, iv2_new(2, 0), SIGN_FROG_3, SPRITE_FROG);
        } break;

        // frog end and sign post
        case 14: {
            place_sign(state, iv2_new(3, 4), SIGN_POST_1, SPRITE_SIGN);
            place_sign(state, iv2_new(4, 4), SIGN_FROG_4, SPRITE_FROG);
        } break;

        // SWAMP AREA

        // T River
        case 50: {
            platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 5, 2, 2, 0, stack);
            platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 4, 2, 2, 2, stack);
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
