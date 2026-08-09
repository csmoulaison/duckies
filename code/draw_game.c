void draw_num(DrawList* draw_list, i32 num, v2 pos) {
    i8 num_ones = num % 10;
    i8 num_tens = num / 10;
    i32 num_palette = 0;
    f32 num_off = 0.0;
    if(num > 99) {
        num_palette = 1;
    }
    if(num > 9) {
        draw_sprite(draw_list, SPRITE_NUMS, num_tens, pos, num_palette);
        num_off = 4.0;
    }
    draw_sprite(draw_list, SPRITE_NUMS, num_ones, v2_add(v2_new(num_off, 0.0), pos), num_palette);
}

u64 sprite_from_tile(Tile tile) {
    switch(tile.type) {
        case TILE_TYPE_GROUND: return SPRITE_GRASS;
        case TILE_TYPE_WATER:  return SPRITE_WATER;
        default: return SPRITE_MAN;
    }
}

void draw_level_tiles(Level* level, DrawList* draw_list, v2 pos_offset) {
	draw_clear_color(draw_list, v4_new(0.2f, 0.2f, 0.2f, 1.0f));
    for(i32 i = 0; i < 64; i++) {
        u32 off_i = i + (i / 8);
        Tile tile = level->tiles[i];
        draw_sprite(draw_list, sprite_from_tile(tile), off_i % 2, v2_add(pixel_pos_from_index(i), pos_offset), 0);
    }
}

v2 entity_draw_pos(DrawList* draw_list, Entity* entity, f32 dt, i32 palette, v2 offset) {
    entity->pos_t += TIME_SCALE * 3.0f * dt;
    if(entity->pos_t > 1.0f) {
        entity->pos_t = 1.0f;
    }
    v2 lerped = v2_new(
        lerp(entity->pos_prev_visible.x, entity->pos_cur.x, entity->pos_t) * 8.0f,
        lerp(entity->pos_prev_visible.y, entity->pos_cur.y, entity->pos_t) * 8.0f);
    return v2_add(lerped, offset);
}

// This is to be used in moments where the game might not be updating the game
// state substantively, but animations and already started moves still run.
// This includes the main game state and during the reset phase.
void update_visual_state(Game* game, DrawList* draw_list, v2 pos_offset, f32 dt) {
    LevelState* state = &game->state;

    // Tiles
    draw_level_tiles(active_game_level(game), draw_list, pos_offset);

    // Platforms
    for(i32 i = 0; i < state->platforms_len; i++) {
        Entity* platform = &state->platforms[i];
        v2 draw_pos = entity_draw_pos(draw_list, platform, dt, 0, pos_offset);
        switch(platform->sink_state) {
            case PLATFORM_FLOAT: {
                draw_sprite_animated_frame_range(draw_list, platform->sprite_handle, ((game->time * 2.0) + 1.0) * 0.5, draw_pos, 0, 1, 0);
            } break;
            case PLATFORM_WARN: {
                draw_sprite(draw_list, platform->sprite_handle, 2, draw_pos, 0);
            } break;
            case PLATFORM_SINK: {
            } break;
            default: panic();
        }
    }

    // Marchers in reverse order.
    for(i32 i = state->marchers_len - 1; i >= 0; i--) {
        Entity* entity = &state->marchers[i];
        v2 draw_pos = entity_draw_pos(draw_list, entity, dt, 0, pos_offset);
        draw_sprite_animated(draw_list, entity->sprite_handle, (game->time + entity->anim_offset_t) * 0.5f, draw_pos, 0);
    }
}

