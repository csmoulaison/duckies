i32 real_chain_index(LevelState* state, i32 relative_chain_index) {
    // gotta do wonky mod to deal with negative numbers
    i32 absolute = (state->move_chain_head + relative_chain_index);
    i32 real = ((absolute % MOVE_CHAIN_SIZE) + MOVE_CHAIN_SIZE) % MOVE_CHAIN_SIZE;
    if(real < 0) {
        printf("real_chain_index calculated as %d from head %d + rel %d = %d\n",
            real, 
            state->move_chain_head, 
            relative_chain_index, 
            state->move_chain_head - relative_chain_index);
        panic();
    }
    return real;
}

iv2 pos_at_relative_chain_index(LevelState* state, i32 chain_index) {
    i32 real_index = real_chain_index(state, chain_index);
    return pos_from_index(state->move_chain[real_index]);
}

bool entity_moved_this_cycle(Entity* entity) {
    return (!iv2_eq(entity->pos_cur, entity->pos_prev));
}

i32 marchers_len(LevelState* state) {
    return state->all_ducks_len + 1;
}

i32 active_marchers_len(LevelState* state) {
    return state->active_ducks_len + 1;
}

bool entity_is_active_marcher(Entity* entity, LevelState* state) {
    for(i32 i = 0; i < active_marchers_len(state); i++) {
        if(entity == &state->marchers[i]) {
            return true;
        }
    }
    return false;
}

bool entity_is_active_duck(Entity* entity, LevelState* state) {
    for(i32 i = 0; i < state->active_ducks_len; i++) {
        if(entity == &state->ducks[i]) {
            return true;
        }
    }
    return false;
}

Level* active_game_level(Game* game) {
    return &game->world->levels[game->level_index];
}

u64 sprite_from_tile(Tile tile) {
    switch(tile.type) {
        case TILE_TYPE_GROUND: return SPRITE_GRASS;
        case TILE_TYPE_WATER:  return SPRITE_WATER;
        default: panic();
    }
}

void draw_level_tiles(Level* level, DrawList* draw_list) {
	draw_clear_color(draw_list, v4_new(0.2f, 0.2f, 0.2f, 1.0f));
    for(i32 i = 0; i < 64; i++) {
        u32 off_i = i + (i / 8);
        Tile tile = level->tiles[i];
        draw_sprite(draw_list, sprite_from_tile(tile), 0, pixel_pos_from_index(i), 0);
    }
}

iv2 pos_from_direction(Entity* entity, MoveDirection move) {
    iv2 position = entity->pos_cur;
    switch(move) {
        case MOVE_UP:    position.y++; break;
        case MOVE_LEFT:  position.x--; break;
        case MOVE_DOWN:  position.y--; break;
        case MOVE_RIGHT: position.x++; break;
        default: break;
    }
    return position;
}

MoveDirection direction_from_delta(iv2 prev, iv2 cur) {
    if(cur.x < prev.x) return MOVE_LEFT;
    if(cur.x > prev.x) return MOVE_RIGHT;
    if(cur.y < prev.y) return MOVE_DOWN;
    if(cur.y > prev.y) return MOVE_UP;
    return MOVE_NONE;
}

MoveDirection entity_direction_from_delta(Entity* entity) {
    iv2 prev = entity->pos_prev;
    iv2 cur  = entity->pos_cur;
    return direction_from_delta(prev, cur);
}

void entity_move_position(LevelState* state, Entity* entity, iv2 position) {
    //assert(!iv2_eq(position, entity->pos_cur));
	entity->pos_prev = entity->pos_cur;
    entity->pos_cur = position;
    entity->pos_t = 0.0f;

    // VOLATILE: If an entity in the active set has called this function, it is
    // assumed to be updating the chain.
    if(entity_is_active_marcher(entity, state)) {
        state->move_chain[real_chain_index(state, entity->chain_index)] = index_from_pos(position);
    }
}

void entity_move_direction(LevelState* state, Entity* entity, MoveDirection move) {
    iv2 position = pos_from_direction(entity, move);
    entity_move_position(state, entity, position);
}

// NOTE: brittle reduntant with below
Entity* try_platform_was_at_pos(Game* game, iv2 pos) {
    LevelState* state = &game->state;
    for(i32 i = 0; i < state->logic_entities_len; i++) {
        Entity* entity = &state->logic_entities[i];
        if(entity->logic_type == LOGIC_MOVING_PLATFORM && iv2_eq(entity->pos_prev, pos)) {
            return entity;
        }
    }
    return NULL;
}

Entity* try_platform_at_pos(Game* game, iv2 pos) {
    LevelState* state = &game->state;
    for(i32 i = 0; i < state->logic_entities_len; i++) {
        Entity* entity = &state->logic_entities[i];
        if(entity->logic_type == LOGIC_MOVING_PLATFORM && iv2_eq(entity->pos_cur, pos)) {
            return entity;
        }
    }
    return NULL;
}

bool pos_safe(Game* game, iv2 pos) {
    Level* level = active_game_level(game);
    Tile tile = tile_from_pos(level, pos);
    if(try_platform_at_pos(game, pos) != NULL) {
        return true;
    }
    if(tile.type == TILE_TYPE_WATER) {
        return false;
    }
    return true;
}

bool pos_passable(Game* game, iv2 pos) {
    return pos_safe(game, pos);
}

bool move_passable(Game* game, Entity* entity, MoveDirection move) {
    iv2 pos = pos_from_direction(entity, move);
    return pos_passable(game, pos);
}

void reset_level(Game* game) {
    game->mode = MODE_LEVEL_RESET;
    game->level_reset_t = 0.0f;
}

void draw_entity(DrawList* draw_list, Entity* entity, f32 time, f32 dt, i32 palette) {
    entity->pos_t += TIME_SCALE * 3.0f * dt;
    if(entity->pos_t > 1.0f) {
        entity->pos_t = 1.0f;
    }
    v2 lerped_pos = v2_new(
        lerp(entity->pos_prev.x, entity->pos_cur.x, entity->pos_t) * 8.0f,
        lerp(entity->pos_prev.y, entity->pos_cur.y, entity->pos_t) * 8.0f);
	entity->pos_visible = v2_lerp(entity->pos_visible, lerped_pos, 24.0f * dt);

    // Draw
    draw_sprite_animated(draw_list, entity->sprite_handle, (time + entity->anim_offset_t) * 0.5f, entity->pos_visible, palette);
    // NOW: debug pos_cur
    //draw_sprite_animated(draw_list, entity->sprite_handle, (time + entity->anim_offset_t) * 0.5f, v2_scale(v2_from_iv2(entity->pos_cur), 8.0f), palette);
    i32 num = entity->chain_index;
    if(num > 9) {
        num = 10;
    }
    draw_sprite(draw_list, SPRITE_NUMS, num, v2_add(entity->pos_visible, v2_new(3.0, 12.0)), 0);
}

// This is to be used in moments where the game might not be updating the game
// state substantively, but animations and already started moves still run.
// This includes the main game state and during the reset phase.
void update_visual_state(Game* game, DrawList* draw_list, f32 dt) {
    LevelState* state = &game->state;

    draw_level_tiles(active_game_level(game), draw_list);

    // Entities, first logic entities, then the chain
    for(i32 i = 0; i < state->logic_entities_len; i++) {
        draw_entity(draw_list, &state->logic_entities[i], game->time, dt, 0);
    }
    // NOW: iterate chain after iterate moving platforms.
    for(i32 i = marchers_len(state) - 1; i >= 0; i--) {
        Entity* entity = &state->marchers[i];
        f32 palette = 0;
        if(!entity_is_active_marcher(entity, state)) {
            palette = 1;
        }
        draw_entity(draw_list, entity, game->time, dt, palette);
    }
}

// This happens all the time, so mustn't be dependant on any state.
// This both plays music and updates the ticks for it.
void update_music_state(Game* game, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
	MusicTrackNote bass_track[48] = { 
        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_FN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_EN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 1), 2.0 },

        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_FN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_EN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 1), 2.0 },

        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_EN, 2), 2.0 },
        { note(N_EN, 2), 2.0 },
        { note(N_EN, 2), 1.0 },
        { note(N_DN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_GN, 2), 2.0 },
        { note(N_GN, 2), 2.0 },
        { note(N_GN, 2), 1.0 },

        { note(N_FN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_BN, 2), 2.0 },
        { note(N_BN, 2), 2.0 },
        { note(N_BN, 2), 2.0 },
        { note(N_AN, 2), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 2), 2.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
	};
	MusicTrackNote melody_track[48] = { 
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_DN, 5), 1.5 },
        { note(N_CS, 5), 1.0 },
        { note(N_BN, 4), 1.0 },
        { note(N_CS, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_EN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },

        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_DN, 5), 1.5 },
        { note(N_CS, 5), 1.0 },
        { note(N_BN, 4), 1.0 },
        { note(N_CS, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_DN, 5), 1.0 },
        { note(N_EN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },

        { note(N000, 0), 0.0 },
        { note(N000, 0), 0.0 },
        { note(N_FN, 5), 0.0 },
        { note(N_AN, 5), 1.2 },
        { note(N_AN, 5), 1.3 },
        { note(N_GN, 5), 1.0 },
        { note(N_FN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_AN, 5), 1.0 },
        { note(N_CN, 6), 1.0 },
        { note(N_CN, 6), 1.0 },
        { note(N_AS, 5), 1.0 },

        { note(N_AN, 5), 1.0 },
        { note(N000, 0), 0.0 },
        { note(N_CN, 6), 1.0 },
        { note(N_EN, 6), 1.0 },
        { note(N_EN, 6), 1.0 },
        { note(N_DN, 6), 1.0 },
        { note(N_AN, 5), 1.0 },
        { note(N_AS, 5), 1.0 },
        { note(N_AN, 5), 1.0 },
        { note(N_GN, 5), 1.0 },
        { note(N_FN, 5), 1.0 },
        { note(N_EN, 5), 1.0 },
	};
	f64 drum_track[12] = { 
        1.2f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.9f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.6f
	};

	f32 i_phase = (f32)((i64)(game->time * 36.0f) % 12) / 12.0f;
	i32 i = ((i64)(game->time * 3.0f) % 48);
	bool hannah_moved_this_cycle = entity_moved_this_cycle(&state->hannah);

    f32 melody_vibrato = 8.0f;
    f32 melody_amp = 0.0f;
    if(hannah_moved_this_cycle) {
        melody_vibrato = 4.0f;
        melody_amp = 0.2f;
    } else {
        for(i32 i = 0; i < 48; i++) {
            bass_track[i].amp *= 0.4;
            bass_track[i].freq *= 2;
        }
    }
    AudioWaveChannel* bass = &audio->wave_channels[0];
    bass->freq = bass_track[i].freq;
    bass->amp  = bass_track[i].amp * 0.25;

    AudioWaveChannel* melody = &audio->wave_channels[1];
    melody->freq = melody_track[i].freq + sinf((f32)game->frames_since_init * 1.0f) * melody_vibrato;
    melody->amp  = (melody_amp - (i_phase * melody_amp) + sinf(i_phase) * 0.2f) * melody_track[i].amp;

    AudioNoiseChannel* noise = &audio->noise_channels[0];
    i32 drum_i = ((i64)(game->time * 3.0f) % 12);
    f32 drum_i_phase = i_phase;
    if(drum_i == 11 && !hannah_moved_this_cycle) {
    	drum_i_phase = (f32)((i64)(game->time * 72.0f) % 12) / 12.0f;
    }
    // Probs extricate some of this from the main music loop.
    noise->amp = clamp(drum_track[drum_i] * 0.166f - drum_i_phase, 0.0f, 1.0f);
    f32 move_t = fmod(game->time, 1.0f);
    if(hannah_moved_this_cycle && move_t < 0.5f) {
        noise->amp += 0.02f + move_t * 0.10f;
    }

    i64 t_old = (i64)(game->time * 2.0f);
	game->time += TIME_SCALE * dt;
    i64 t_new = (i64)(game->time * 2.0f);
    if((t_old % 2 == 0 && t_new % 2 == 1) || (t_old % 2 == 1 && t_new % 2 == 0)) {
        game->cycle_stage_counter++;
        if(game->cycle_stage_counter > 1) {
            game->new_cycle_this_frame = true;
            game->half_cycle_this_frame = false;
            hannah_moved_this_cycle = false;
            game->cycle_stage_counter = 0;
        } else if(game->cycle_stage_counter <= 1) {
            game->half_cycle_this_frame = true;
            game->new_cycle_this_frame = false;
        }
    } else {
        game->new_cycle_this_frame = false;
        game->half_cycle_this_frame = false;
    }
}

