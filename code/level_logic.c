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
    i32 steps_one_way = x2 - x1 - 1;
    i32 steps_both_ways = (steps_one_way + steps_one_way);
    i32 extra_end_steps = (per_end_count) * 2;
    i32 len = steps_both_ways * per_step_count + extra_end_steps;
    //printf("one %d both %d extra %d\n", steps_one_way, steps_both_ways, extra_end_steps);
    
    iv2* positions = (iv2*)stack_alloc(stack, len * sizeof(iv2));
    {
        i32 i = 0;
        for(i32 e = 0; e < per_end_count; e++) {
            //printf("  e1: %d\n", i);
            positions[(i + index_offset) % len] = iv2_new(x1, y);
            i++;
        }
        for(i32 x = x1 + 1; x < x2; x++) {
            for(i32 j = 0; j < per_step_count; j++) {
                //printf("  x1 %d: %d\n", j, i);
                positions[(i + index_offset) % len] = iv2_new(x, y);
                i++;
            }
        }
        for(i32 e = 0; e < per_end_count; e++) {
            //printf("  e2: %d\n", i);
            positions[(i + index_offset) % len] = iv2_new(x2, y);
            i++;
        }
        for(i32 x = x2 - 1; x > x1; x--) {
            for(i32 j = 0; j < per_step_count; j++) {
                //printf("  x2 %d: %d\n", j, i);
                positions[(i + index_offset) % len] = iv2_new(x, y);
                i++;
            }
        }
        //printf("i %d len %d\n", i, len);
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
            i32 len = platform_run_x_back_forth_sequence(game, 3, 0, 5, 4, 2, 2, 2, stack);
                      platform_run_x_back_forth_sequence(game, 3, 0, 5, 3, 2, 2, 6, stack);
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
