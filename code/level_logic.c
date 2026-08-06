void platform_follow_positions(Game* game, i32 logic_id, iv2* positions, i32 len) {
    LevelState* state = &game->state;
    Entity* platform = &state->logic_entities[logic_id];
    i64 time_i = (i64)game->time;
    iv2 target = positions[(i32)time_i % len];

    if(!iv2_eq(platform->pos_cur, target)) {
        entity_move(state, platform, direction_from_target(platform, target));
    } else {
        platform->move_this_cycle = MOVE_NONE;
    }
    platform->sprite_handle = SPRITE_LILY;
    platform->logic_type = LOGIC_MOVING_PLATFORM;
}

void pre_update_level_logic(Game* game) {
    LevelState* state = &game->state;
    if(game->new_cycle_this_frame) {
        for(i32 i = 0; i < state->logic_entities_len; i++) {
            state->logic_entities[i].pos_prev_visible = v2_from_iv2(state->logic_entities[i].pos_cur);
        }
    }
    switch(game->level_index) {
        case 0: {
            iv2 platform_positions[12] = {
                {{ 1, 2 }},
                {{ 2, 2 }},
                {{ 3, 2 }},
                {{ 4, 2 }},
                {{ 5, 2 }},
                {{ 6, 2 }},
                {{ 6, 2 }},
                {{ 5, 2 }},
                {{ 4, 2 }},
                {{ 3, 2 }},
                {{ 2, 2 }},
                {{ 1, 2 }}
            };
            iv2 platform_positions_2[12] = {
                {{ 2, 2 }},
                {{ 3, 2 }},
                {{ 4, 2 }},
                {{ 5, 2 }},
                {{ 6, 2 }},
                {{ 7, 2 }},
                {{ 7, 2 }},
                {{ 6, 2 }},
                {{ 5, 2 }},
                {{ 4, 2 }},
                {{ 3, 2 }},
                {{ 2, 2 }},
            };
            state->logic_entities_len = 2;
            platform_follow_positions(game, 0, platform_positions, 12);
            platform_follow_positions(game, 1, platform_positions_2, 12);
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
