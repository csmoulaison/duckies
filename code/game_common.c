i32 ducks_len(LevelState* state) {
    return state->marchers_len - 1;
}

Level* active_game_level(Game* game) {
    return &game->world->levels[game->level_index];
}

iv2 delta_from_direction(MoveDirection dir) {
    // NOW: must make sure its only one direction here.
    iv2 pos = {};
    switch(dir) {
        case MOVE_UP:    pos.y++; break;
        case MOVE_LEFT:  pos.x--; break;
        case MOVE_DOWN:  pos.y--; break;
        case MOVE_RIGHT: pos.x++; break;
        default: break;
    }
    return pos;
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

MoveDirection direction_from_target(Entity* entity, iv2 target) {
    return direction_from_delta(entity->pos_cur, target);
}

iv2 pos_after_direction(Entity* entity, MoveDirection dir) {
    iv2 pos = entity->pos_cur;
    switch(dir) {
        case MOVE_UP:    pos.y++; break;
        case MOVE_LEFT:  pos.x--; break;
        case MOVE_DOWN:  pos.y--; break;
        case MOVE_RIGHT: pos.x++; break;
        default: break;
    }
    return pos;
}

// This is used for simulation as well, so shold never use state.
void entity_move(Entity* entity, MoveDirection move) {
    if(move == MOVE_NONE) {
        return;
    }

    iv2 pos = pos_after_direction(entity, move);
    entity->pos_prev = entity->pos_cur;
    entity->pos_lead = entity->pos_prev;
    entity->pos_cur = pos;
    entity->pos_t = 0.0f;
    entity->move_this_cycle = move;
}

// NOTE: brittle reduntant with below
Entity* try_platform_was_at_pos(Game* game, iv2 pos) {
    LevelState* state = &game->state;
    for(i32 i = 0; i < state->platforms_len; i++) {
        Entity* platform = &state->platforms[i];
        if(iv2_eq(platform->pos_prev, pos)) {
            return platform;
        }
    }
    return NULL;
}

Entity* try_platform_at_pos(Game* game, iv2 pos) {
    LevelState* state = &game->state;
    for(i32 i = 0; i < state->platforms_len; i++) {
        Entity* platform = &state->platforms[i];
        if(iv2_eq(platform->pos_cur, pos)) {
            return platform;
        }
    }
    return NULL;
}

bool pos_safe(Game* game, iv2 pos) {
    Level* level = active_game_level(game);
    Tile tile = tile_from_pos(level, pos);
    Entity* platform = try_platform_at_pos(game, pos);
    if(platform != NULL) {
        return (platform->sink_state != PLATFORM_SINK);
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
    iv2 pos = pos_after_direction(entity, move);
    return pos_passable(game, pos);
}

void reset_level(Game* game) {
    game->mode = MODE_LEVEL_RESET;
    game->level_reset_t = 0.0f;
}

void override_pallete_from_fade_t(DrawList* list, f32 t) {
    if(t > 0.40) list->palette_override_index = 1;
    if(t > 0.44) list->palette_override_index = 2;
    if(t > 0.76) list->palette_override_index = 1;
    if(t > 0.80) list->palette_override_index = 0;
}
