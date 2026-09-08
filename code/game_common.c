i32 palette_from_marcher_index(i32 i) {
    i32 pl = 0;
    if(i > 1) {
        pl = 8 + i - 2;
    }
    return pl;
}

i32 ducks_len(LevelState* state) {
    return state->marchers_len - 1;
}

Level* prev_game_level(Game* game) {
    return &game->world->levels[game->state.level_index_prev];
}

Level* active_game_level(Game* game) {
    return &game->world->levels[game->state.level_index];
}

bool pos_adjacent(iv2 a, iv2 b) {
    bool adjacent = false;
    if(iv2_eq(b, iv2_new(a.x - 1, a.y))) adjacent = true;
    if(iv2_eq(b, iv2_new(a.x + 1, a.y))) adjacent = true;
    if(iv2_eq(b, iv2_new(a.x, a.y - 1))) adjacent = true;
    if(iv2_eq(b, iv2_new(a.x, a.y + 1))) adjacent = true;
    return adjacent;
}

bool any_honk_at_pos(LevelState* state, iv2 pos) {
    for(i32 i = 0; i < ducks_len(state); i++) {
        Entity* duck = &state->ducks[i];
        if(duck->honk_this_cycle && iv2_eq(duck->pos_cur, pos)) {
            return true;
        }
    }
    return false;
}

bool any_honk_adjacent(LevelState* state, iv2 pos) {
    for(i32 i = 0; i < ducks_len(state); i++) {
        Entity* duck = &state->ducks[i];
        if(duck->honk_this_cycle && pos_adjacent(duck->pos_cur, pos)) {
            return true;
        }
    }
    return false;
}

bool any_marcher_at_pos(LevelState* state, iv2 pos) {
    for(i32 i = 0; i < state->marchers_len; i++) {
        if(iv2_eq(pos, state->marchers[i].pos_cur)) {
            return true;
        }
    }
    return false;
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

bool pos_in_bounds(iv2 pos) {
    return pos.x > -1 && pos.x < 8 && pos.y > -1 && pos.y < 8;
}

iv2 relative_pos_after_direction(iv2 pos, MoveDirection dir) {
    switch(dir) {
        case MOVE_UP:    pos.y++; break;
        case MOVE_LEFT:  pos.x--; break;
        case MOVE_DOWN:  pos.y--; break;
        case MOVE_RIGHT: pos.x++; break;
        default: break;
    }
    return pos;
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

void entity_offset_teleport(Entity* entity, iv2 offset) {
    v2 offset_v2 = v2_from_iv2(offset);
    entity->pos_prev = iv2_add(entity->pos_prev, offset);
    entity->pos_lead = iv2_add(entity->pos_lead, offset);
    entity->pos_cur = iv2_add(entity->pos_cur, offset);
    entity->pos_visible = v2_add(entity->pos_visible, offset_v2);
    entity->pos_prev_visible = v2_add(entity->pos_prev_visible, offset_v2);
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

void entity_place(Entity* entity, iv2 pos) {
    entity->move_this_cycle = MOVE_NONE;
    entity->pulled_move_this_cycle = MOVE_NONE;
    entity->pos_t = 1.0;

    entity->pos_cur = pos;
    entity->pos_prev = pos;
    entity->pos_lead = pos;

    v2 fpos = v2_scale(v2_from_iv2(pos), 8.0);
    entity->pos_visible = fpos;
    entity->pos_prev_visible = fpos;
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

// Awkward name, its a jam.
typedef struct {
    Level* self;
    Level* up;
    Level* left;
    Level* down;
    Level* right;
    Level* ul;
    Level* ur;
    Level* dl;
    Level* dr;
} LevelPacket;

LevelPacket make_level_packet(Game* game, Level* level) {
    LevelPacket pk = {};
    pk.self = level;
    if(level->exit_up != 0) {
        pk.up = &game->world->levels[level->exit_up];
        if(pk.up->exit_left != 0) {
            pk.ul = &game->world->levels[pk.up->exit_left];
        }
        if(pk.up->exit_right!= 0) {
            pk.ur = &game->world->levels[pk.up->exit_right];
        }
    }
    if(level->exit_left != 0) {
        pk.left = &game->world->levels[level->exit_left];
    }
    if(level->exit_down != 0) {
        pk.down = &game->world->levels[level->exit_down];
        if(pk.down->exit_left != 0) {
            pk.dl = &game->world->levels[pk.down->exit_left];
        }
        if(pk.down->exit_right!= 0) {
            pk.dr = &game->world->levels[pk.down->exit_right];
        }
    }
    if(level->exit_right != 0) {
        pk.right = &game->world->levels[level->exit_right];
    }
    return pk;
}

Tile tile_from_pos_and_level_packet(iv2 pos, LevelPacket* levels) {
    Level* check_level = levels->self;
    Tile def = (Tile){ .type = TILE_TYPE_GROUND, .flags = 0, .subtype = 0 };
    if(pos.y < 0) {
        pos.y += 8;
        if(pos.x < 0) {
            if(levels->dl != NULL) {
                pos.x += 8;
                check_level = levels->dl;
            } else {
                return def;
            }
        } else if(pos.x > 7) {
            if(levels->dr != NULL) {
                pos.x -= 8;
                check_level = levels->dr;
            } else {
                return def;
            }
        } else {
            if(levels->down != NULL) {
                check_level = levels->down;
            } else {
                return def;
            }
        }
    } else if(pos.y > 7) {
        pos.y -= 8;
        if(pos.x < 0) {
            if(levels->ul != NULL) {
                pos.x += 8;
                check_level = levels->ul;
            } else {
                return def;
            }
        } else if(pos.x > 7) {
            if(levels->ur != NULL) {
                pos.x -= 8;
                check_level = levels->ur;
            } else {
                return def;
            }
        } else {
            if(levels->up != NULL) {
                check_level = levels->up;
            } else {
                return def;
            }
        }
    } else if(pos.x < 0) {
        if(levels->left != NULL) {
            pos.x += 8;
            check_level = levels->left;
        } else {
            return def;
        }
    } else if(pos.x > 7) {
        if(levels->right != NULL) {
            pos.x -= 8;
            check_level = levels->right;
        } else {
            return def;
        }
    }
    return check_level->tiles[index_from_pos(pos)];
}

Tile tile_from_pos(Game* game, Level* level, iv2 pos) {
    LevelPacket packet = make_level_packet(game, level);
    return tile_from_pos_and_level_packet(pos, &packet);
}

bool pos_safe(Game* game, iv2 pos) {
    Level* level = active_game_level(game);
    Tile tile = tile_from_pos(game, level, pos);
    if(tile.type == TILE_TYPE_WATER) {
        Entity* platform = try_platform_at_pos(game, pos);
        if(platform != NULL) {
            if(platform->sink_state == PLATFORM_SINK) 
                return false;
            if(platform->crumbler && platform->hits_taken > platform->max_health) 
                return false;
            return true;
        } 
        return false;
    }
    return true;
}

iv2 muffin_pos_from_index(i32 i) {
    iv2 pos = iv2_new(1 + (i % 3) * 2, 1 + (i / 3) * 2);
    return pos;
}

bool pos_passable(Game* game, iv2 pos) {
    Level* level = active_game_level(game);
    LevelState* state = &game->state;
    bool passable = !(tile_from_pos(game, level, pos).flags & TILE_FLAG_CLIFF);
    for(i32 i = 0; i < state->gates_len; i++) {
        Gate* gate = &state->gates[i];
        if(iv2_eq(pos, gate->pos) && gate->state != GATE_OPEN) {
            passable = false;
            break;
        }
    }
    if(state->level_index == 67) {
        for(i32 i = 0; i < 9; i++) {
            if(iv2_eq(pos, muffin_pos_from_index(i))) {
                passable = false;
                break;
            }
        }
    }
    return passable;
}

bool move_passable(Game* game, Entity* entity, MoveDirection move) {
    iv2 pos = pos_after_direction(entity, move);
    return pos_passable(game, pos);
}

void reset_level(Game* game) {
    game->mode = MODE_LEVEL_RESET;
    game->level_reset_t = 0.0f;
}

void override_pallete_from_fade_t_parameters(DrawList* list, f32 t, f32 t1, f32 t2, f32 t3, f32 t4) {
    list->palette_override_index = 0;
    if(t > t1) list->palette_override_index = 1;
    if(t > t2) list->palette_override_index = 2;
    if(t > t3) list->palette_override_index = 1;
    if(t > t4) list->palette_override_index = 0;

}

void override_pallete_from_fade_one_way_t(DrawList* list, f32 t) {
    list->palette_override_index = 2;
    if(t > 0.50) list->palette_override_index = 1;
    if(t > 0.75) list->palette_override_index = 0;
}

void override_pallete_from_fade_t(DrawList* list, f32 t) {
    override_pallete_from_fade_t_parameters(list, t, 0.40, 0.44, 0.76, 0.80);
}

void update_input_move(Game* game) {
    LevelState* state = &game->state;

    Entity* hannah = &state->hannah;
    if(state->boat_riding) {
        state->input_move = MOVE_LEFT;
        hannah->sprite_handle = SPRITE_HANNAH_LEFT;
        return;
    }

	if(input_button_pressed(game->input_buttons[BUTTON_UP])) {
    	state->input_move = MOVE_UP;
    	hannah->sprite_handle = SPRITE_HANNAH_UP;
    	game->first_move_made = true;
	}
	else if(input_button_pressed(game->input_buttons[BUTTON_LEFT])) {
    	state->input_move = MOVE_LEFT;
    	hannah->sprite_handle = SPRITE_HANNAH_LEFT;
    	game->first_move_made = true;
	}
	else if(input_button_pressed(game->input_buttons[BUTTON_DOWN])) {
    	state->input_move = MOVE_DOWN;
    	hannah->sprite_handle = SPRITE_HANNAH_DOWN;
    	game->first_move_made = true;
	}
	else if(input_button_pressed(game->input_buttons[BUTTON_RIGHT])) {
    	state->input_move = MOVE_RIGHT;
    	hannah->sprite_handle = SPRITE_HANNAH_RIGHT;
    	game->first_move_made = true;
	}

	if(state->honking_unlocked && input_button_pressed(game->input_buttons[BUTTON_EDITOR_PLACE])) {
    	bool can_honk = true;
    	for(i32 i = 0; i < state->marchers_len - 1; i++) {
        	if(state->marchers[i].honk_this_cycle) {
            	can_honk = false;
            	break;
        	}
    	}
    	if(can_honk) {
            state->input_honk = true;
    	}
	}
}

