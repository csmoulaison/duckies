Entity* push_platform(LevelState* state) {
    Entity* platform = &state->platforms[state->platforms_len];
    state->platforms_len++;
    platform->sprite_handle = SPRITE_LILY;
    return platform;
}

void platform_set_pos(Game* game, Entity* platform, iv2 pos) {
    platform->pos_cur = pos;
    platform->pos_prev = pos;
    platform->pos_prev_visible = v2_from_iv2(pos);
}

void platform_sink_cycle(Game* game, Entity* platform, PlatformSinkState* cycle, i32 len, i32 offset) { 
    LevelState* state = &game->state;
    i32 i = (state->cycle_index + offset) % len;
    platform->sink_state = cycle[i];
}

void platform_follow_sequence(Game* game, Entity* platform, iv2* positions, i32 len, i32 offset) {
    LevelState* state = &game->state;
    //i64 time_i = (i64)game->time;
    //i32 pos_i = (i32)time_i % len;
    i32 pos_i = (state->cycle_index + offset) % len;
    iv2 target = positions[pos_i];

    f32 target_dist = iv2_distance(platform->pos_cur, target);
    if(game->new_cycle_this_frame) {
        if(!within_epsilon(target_dist, 0.0, 0.1) && !within_epsilon(target_dist, 1.0, 0.1)) {
            platform->pos_cur = target;
            platform->pos_prev = target;
            platform->pos_prev_visible = v2_from_iv2(target);
        } else {
            if(!iv2_eq(platform->pos_cur, target)) {
                entity_move(platform, direction_from_target(platform, target));
            } else {
                platform->move_this_cycle = MOVE_NONE;
            }
        }
    }
}

i32 platform_x_back_forth_sequence(
    Game* game, 
    Entity* platform,
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

    platform_follow_sequence(game, platform, positions, len, 0);
    return len;
}

// return len
i32 platform_push_run_x_back_forth_sequence(
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
        len = platform_x_back_forth_sequence(game, push_platform(&game->state), x1 + i, x2 + i, y, per_end_count, per_step_count, index_offset, stack);
    }
    return len;
}

