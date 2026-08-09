#include "level_logic_common.c"

void pre_update_level_logic(Game* game, Stack* stack) {
    LevelState* state = &game->state;

    // Default level exits to -1
    if(game->new_cycle_this_frame) {
        for(i32 i = 0; i < state->platforms_len; i++) {
            state->platforms[i].pos_prev_visible = v2_from_iv2(state->platforms[i].pos_cur);
        }
    }
    state->platforms_len = 0;
    switch(state->level_index) {
        case 0: { // mult plats
            i32 len = platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 4, 2, 2, 0, stack);
                      platform_push_run_x_back_forth_sequence(game, 3, 0, 5, 3, 2, 2, 2, stack);
        } break;

        case 1: { // sink cycvl3
            PlatformSinkState cycle[5] = { 
                PLATFORM_FLOAT,
                PLATFORM_WARN,
                PLATFORM_SINK,
                PLATFORM_SINK,
                PLATFORM_WARN
            };
            // This for loop could have a set_pos_rect helper and just be an i
            for(i32 x = 0; x < 2; x++) {
                for(i32 y = 0; y < 2; y++) {
                    Entity* plat = push_platform(state);
                    platform_sink_cycle(game, plat, cycle, 5, 1);
                    platform_set_pos(game, plat, iv2_new(3 + x, 3 + y));
                }
            }
        } break;

        case 2: { // snake test
            iv2 positions[16] = {
                {{ -3, 3 }},
                {{ -2, 3 }},
                {{ -1, 3 }},
                {{ 0, 3 }},

                {{ 1, 3 }},
                {{ 2, 3 }},
                {{ 3, 3 }},
                {{ 4, 3 }},

                {{ 5, 3 }},
                {{ 5, 4 }},
                {{ 5, 5 }},
                {{ 5, 6 }},

                {{ 5, 7 }},
                {{ 5, 8 }},
                {{ 5, 9 }},
                {{ 5, 10 }}
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

        default: break;
    }
}

void post_update_level_logic(Game* game) {
    LevelState* state = &game->state;
    switch(state->level_index) {
        default: break;
    }
}
