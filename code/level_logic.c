Entity* push_entity(LevelState* state) {
    state->logic_entities_len++;
    return &state->logic_entities[state->logic_entities_len - 1];
}

i32 platform_x_back_forth_sequence(
    Game* game, 
    i32 x1, 
    i32 x2, 
    i32 y, 
    i32 per_end_count, 
    i32 per_step_count,
    i32 index_offset, 
    Stack* stack)
{
    i32 len = (((x2 - x1 + 1) * 2) - 1) * per_step_count + ((per_end_count - 1) * 2);
    iv2* positions = (iv2*)stack_alloc(stack, len * sizeof(iv2));
    {
        i32 i;
        for(i = 0; i < per_end_count - 1; i++) {
            positions[(i + index_offset) % len] = iv2_new(x1, y);
        }
        for(i32 x = x1; x <= x2; x++) {
            for(i32 j = 0; j < per_step_count; j++) {
                positions[(i + index_offset) % len] = iv2_new(x, y);
                i++;
            }
        }
        for(i = i; i < per_end_count - 1; i++) {
            positions[(i + index_offset) % len] = iv2_new(x2, y);
        }
        for(i32 x = x2; x > x1; x--) {
            for(i32 j = 0; j < per_step_count; j++) {
                positions[(i + index_offset) % len] = iv2_new(x, y);
                i++;
            }
        }
        printf("i %d len %d\n", i, len);
        assert(i == len);
    }

    LevelState* state = &game->state;
    Entity* platform = push_entity(state);
    //i64 time_i = (i64)game->time;
    //i32 pos_i = (i32)time_i % len;
    i32 pos_i = state->cycle_index % len;
    iv2 target = positions[pos_i];

    f32 target_dist = iv2_distance(platform->pos_cur, target);
    if(target_dist != 0.0 && target_dist != 1.0) {
        platform->pos_cur = target;
        platform->pos_prev = target;
        platform->pos_prev_visible = v2_from_iv2(target);
    } else if(!iv2_eq(platform->pos_cur, target) && game->new_cycle_this_frame) {
        entity_move(platform, direction_from_target(platform, target));
    } else {
        platform->move_this_cycle = MOVE_NONE;
    }
    platform->sprite_handle = SPRITE_LILY;
    platform->logic_type = LOGIC_MOVING_PLATFORM;

    return len;
}

// return len
i32 platform_run_x_back_forth_sequence(
    Game* game, 
    i32 count, 
    i32 x1, 
    i32 x2, 
    i32 y, 
    i32 per_end_count,
    i32 per_step_count,
    i32 index_offset, 
    Stack* stack)
{
    i32 len = -1;
    for(i32 i = 0; i < count; i++) {
        len = platform_x_back_forth_sequence(game, x1 + i, x2 + i, y, per_end_count, per_step_count, index_offset, stack);
    }
    return len;
}

void pre_update_level_logic(Game* game, Stack* stack) {
    LevelState* state = &game->state;
    if(game->new_cycle_this_frame) {
        for(i32 i = 0; i < state->logic_entities_len; i++) {
            state->logic_entities[i].pos_prev_visible = v2_from_iv2(state->logic_entities[i].pos_cur);
        }
    }
    state->logic_entities_len = 0;
    switch(game->level_index) {
        case 0: {
            i32 len = platform_run_x_back_forth_sequence(game, 3, 0, 5, 4, 1, 1, 2, stack);
                      platform_run_x_back_forth_sequence(game, 3, 0, 5, 3, 1, 2, len / 2, stack);
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
