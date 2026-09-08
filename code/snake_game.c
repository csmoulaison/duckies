void update_snake_minigame(Game* game) {
    LevelState* state = &game->state;
    SnakeGame* minigame = &state->snake_game;

    if(state->level_index != 36) {
        return;
    }
    if(minigame->mode == SNAKE_GAME_INACTIVE) {
        return;
    }
    if(minigame->mode == SNAKE_GAME_LOST) {
        return;
    }
    if(minigame->mode == SNAKE_GAME_WON) {
        return;
    }


    if(game->new_cycle_this_frame) {
        for(i32 i = 0; i < 36; i++) {
            Entity* platform = &state->platforms[i];
            bool match_pos = false;
            for(i32 j = 0; j < state->marchers_len - 1; j++) {
                Entity* marcher = &state->marchers[j];
                if(iv2_eq(marcher->pos_cur, platform->pos_cur)) {
                    match_pos = true;
                    platform->sink_state = PLATFORM_FAKE_WARN;
                }
            }
            Entity* final_marcher = &state->marchers[state->marchers_len - 1];
            if(iv2_eq(final_marcher->pos_cur, platform->pos_cur)) {
                match_pos = true;
                if(platform->sink_state == PLATFORM_FAKE_WARN || platform->sink_state == PLATFORM_FLOAT) {
                    platform->sink_state = PLATFORM_WARN;
                } else {
                    platform->sink_state = PLATFORM_SINK;
                    platform->reappear_countdown = minigame->snake_extra_length;
                }
            }
            if(iv2_eq(final_marcher->pos_prev, platform->pos_cur)) {
                match_pos = true;
                platform->sink_state = PLATFORM_SINK;
                platform->reappear_countdown = minigame->snake_extra_length;
            }
            if(!match_pos) {
                platform->reappear_countdown--;
                if(platform->reappear_countdown <= 0) {
                    platform->sink_state = PLATFORM_FLOAT;
                }
            }
        }

        Entity* hannah = &state->marchers[0];
        if(minigame->apple_active && iv2_eq(hannah->pos_cur, minigame->apple_pos)) {
            printf("eaden apl\n");
            minigame->snake_extra_length++;
            minigame->apple_pos = iv2_new(1 + random_i32(5), 1 + random_i32(5));
            //minigame->apple_active = false;
            //minigame->apple_transition_t = 0.0;
        }
    }
}
