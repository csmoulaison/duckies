// Common entity
void entity_follow_target(Game* game, Entity* entity, iv2 target) {
    LevelState* state = &game->state;
    f32 target_dist = iv2_distance(entity->pos_cur, target);
    if(game->new_cycle_this_frame) {
        if(!within_epsilon(target_dist, 0.0, 0.1) && !within_epsilon(target_dist, 1.0, 0.1)) {
            entity->pos_cur = target;
            entity->pos_prev = target;
            entity->pos_prev_visible = v2_from_iv2(target);
        } else {
            if(!iv2_eq(entity->pos_cur, target)) {
                entity_move(entity, direction_from_target(entity, target));
            } else {
                entity->move_this_cycle = MOVE_NONE;
            }
        }
    }
}

// Buttons
Button* place_button(LevelState* state, iv2 pos) {
    Button* button = &state->buttons[state->buttons_len];
    state->buttons_len++;
    button->pos = pos;
    return button;
}

// Sign
void place_sign(LevelState* state, iv2 pos, i32 index, i64 sprite) {
    Sign* sign = &state->signs[state->signs_len];
    state->signs_len++;

    sign->index = index;
    sign->pos = pos;
    sign->sprite = sprite;
}

// Gates
Gate* place_button_gate(LevelState* state, iv2 pos, i32* button_indices, i32 buttons_len, i32 permagate_index) {
    Gate* gate = &state->gates[state->gates_len];
    state->gates_len++;
    gate->pos = pos;
    gate->trigger_type = TRIGGER_BUTTONS;
    gate->trigger.buttons.len = buttons_len;
    gate->permagate_index = permagate_index;
    for(i32 i = 0; i < buttons_len; i++) {
        gate->trigger.buttons.indices[i] = button_indices[i];
    }
    return gate;
}

// Egg
void place_egg(LevelState* state, iv2 pos, i32 egg_index) {
    state->egg_exists = true;
    state->egg_index = egg_index;
    state->egg_pos = pos;
}

// Car
// palette is not a 1:1 with actual palettes.
i32 car_palette_map[3] = { 0, 5, 6 };
Entity* push_car(LevelState* state, i64 sprite, i32 width, i32 car_palette) {
    Entity* car = &state->cars[state->cars_len];
    state->cars_len++;
    car->sprite_handle = sprite;
    car->car_width = width;
    car->palette_swap = car_palette_map[car_palette];
    return car;
}

void platform_car_cycle_rtl(Game* game, Entity* car, i32 x, i32 y, i32 cycle_len) {
    LevelState* state= &game->state;
    i32 pos_i = (state->cycle_index + x) % cycle_len;
    iv2 target = iv2_new(7 + car->car_width - pos_i, y);
    entity_follow_target(game, car, target);
}

void platform_car_cycle_ltr(Game* game, Entity* car, i32 x, i32 y, i32 cycle_len) {
    LevelState* state= &game->state;
    i32 pos_i = (state->cycle_index + x) % cycle_len;
    iv2 target = iv2_new(0 - car->car_width + pos_i, y);
    entity_follow_target(game, car, target);
}

// Platforms
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
    i32 pos_i = (state->cycle_index + offset) % len;
    iv2 target = positions[pos_i];
    entity_follow_target(game, platform, target);
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

// returns len
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

