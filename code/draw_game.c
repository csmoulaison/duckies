#include "draw_tiles.c"

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

u64 type_sprite_from_tile(Tile tile) {
    switch(tile.type) {
        case TILE_TYPE_GROUND: {
            switch(tile.subtype) {
                case 0: return SPRITE_GRASS;
                case 1: return SPRITE_ROAD;
                default: return SPRITE_MAN;
            }
        } break;
        case TILE_TYPE_WATER:  return SPRITE_WATER;
        default: return SPRITE_MAN;
    }
}

u64 flag_sprite_from_tile(Tile tile) {
    if(tile.flags & TILE_FLAG_CLIFF) {
        return SPRITE_CLIFF;
    }
    return -1;
}

void draw_level_tiles(Game* game, Level* level, DrawList* draw_list, v2 pos_offset) {
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

	draw_clear_color(draw_list, v4_new(0.2f, 0.2f, 0.2f, 1.0f));
    for(i32 i = 0; i < 64; i++) {
        Tile tile = level->tiles[i];
        u32 off_i = i + (i / 8);
        i32 type_pl = 0;
        i32 cap_pl = 4;
        if(off_i % 2 == 1) {
            type_pl = 4;
            cap_pl = 0;
        }

        i32 cliff_frame = 0;
        i32 cap_frame = -1; 
        bool tile_cliff = (tile.flags & TILE_FLAG_CLIFF);
        if(tile_cliff) {
            cliff_frame = tile_cliff_frame(pos_from_index(i), &pk, &cap_frame);
        }

        v2 draw_pos = v2_add(pixel_pos_from_index(i), pos_offset);
        draw_sprite(draw_list, type_sprite_from_tile(tile), 0, draw_pos, type_pl);

        if(tile_cliff) {
            draw_sprite(draw_list, flag_sprite_from_tile(tile), cliff_frame, draw_pos, 0);
            if(cap_frame != -1) {
                draw_sprite_layer(draw_list, SPRITE_CLIFF, cap_frame, v2_add(draw_pos, v2_new(0.0, 8.0)), 0, 1);
            }
        }
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
    f32 anim_t = ((game->time * 2.0) + 1.0) * 0.5;

    // Tiles
    draw_level_tiles(game, active_game_level(game), draw_list, pos_offset);

    // Platforms
    for(i32 i = 0; i < state->platforms_len; i++) {
        Entity* platform = &state->platforms[i];
        v2 draw_pos = entity_draw_pos(draw_list, platform, dt, 0, pos_offset);
        switch(platform->sink_state) {
            case PLATFORM_FLOAT: {
                draw_sprite_animated_frame_range(draw_list, platform->sprite_handle, anim_t, draw_pos, 0, 1, 0);
            } break;
            case PLATFORM_WARN: {
                draw_sprite(draw_list, platform->sprite_handle, 2, draw_pos, 0);
            } break;
            case PLATFORM_SINK: {
            } break;
            default: panic();
        }
    }

    // Cars
    for(i32 i = 0; i < state->cars_len; i++) {

        Entity* car = &state->cars[i];
        v2 draw_pos = entity_draw_pos(draw_list, car, dt, 0, pos_offset);
        draw_sprite_animated_frame_range(draw_list, car->sprite_handle, anim_t, draw_pos, 0, 1, 0);
    }

    // Egg
    if(state->level_egg_exists) {
        v2 egg_draw_pos = v2_scale(v2_from_iv2(state->level_egg_pos), 8.0);
        egg_draw_pos = v2_add(egg_draw_pos, pos_offset);
        if(state->level_egg_collected) {
            draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 6, egg_draw_pos, 0);
        } else {
            if(state->level_egg_broken) {
                if(state->level_egg_t > 1.0) {
                    state->level_egg_t = 0.99;
                }
            }
            draw_sprite_animated_frame_range(draw_list, SPRITE_DUCK_EXPLODE, state->level_egg_t, egg_draw_pos, 0, 5, 0);
        } 
    }

    // Frog
    if(state->frog_exists) {
        v2 frog_draw_pos = v2_scale(v2_from_iv2(state->frog_pos), 8.0);
        frog_draw_pos = v2_add(frog_draw_pos, pos_offset);
        draw_sprite(draw_list, SPRITE_FROG, 0, frog_draw_pos, 0);
    }

    // Marchers in reverse order.
    for(i32 i = state->marchers_len - 1; i >= 0; i--) {
        Entity* entity = &state->marchers[i];
        v2 draw_pos = entity_draw_pos(draw_list, entity, dt, 0, pos_offset);
        draw_sprite_animated(draw_list, entity->sprite_handle, (game->time + entity->anim_offset_t) * 0.5f, draw_pos, 0);
    }
}

