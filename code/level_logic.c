void pre_update_level_logic(Game* game) {
    LevelState* state = &game->state;
    for(i32 i = 0; i < state->logic_entities_len; i++) {
        state->logic_entities[i].pos_prev = state->logic_entities[i].pos_cur;
    }

    switch(game->level_index) {
        case 0: {
            iv2 platform_positions[6] = {
                {{ 4, 2 }},
                {{ 4, 3 }},
                {{ 4, 4 }},
                {{ 4, 5 }},
                {{ 4, 4 }},
                {{ 4, 3 }},
            };
            // NOW: switch to push_entity model
            state->logic_entities_len = 1;
            Entity* platform = &state->logic_entities[0];
            i64 time_i = (i64)game->time;
            iv2 target_pos = platform_positions[(i32)time_i % 6];

            if(!iv2_eq(platform->pos_cur, target_pos)) {
                entity_move_position(state, platform, target_pos);
            }
            platform->sprite_handle = SPRITE_LILY;
            platform->logic_type = LOGIC_MOVING_PLATFORM;
        } break;
        default: break;
    }
}

void post_update_level_logic(Game* game) {
    LevelState* state = &game->state;
    switch(game->level_index) {
        default: break;
    }
}
