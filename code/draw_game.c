#include "draw_tiles.c"

#define BUTTON_PRESS_TIME 0.5
#define GATE_OPEN_TIME 0.75

typedef struct {
    i32 sprite;
    i32 frame;
    i32 palette;
} TileDrawInfo;

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

TileDrawInfo tile_draw_info(Tile tile, i32 i) {
    switch(tile.type) {
        case TILE_TYPE_GROUND: {
            switch(tile.subtype) {
                // Grass
                case 0: {
                    u32 off_i = i + (i / 8);
                    i32 pl = 0;
                    if(off_i % 2 == 1) pl = 4;
                    return (TileDrawInfo){ SPRITE_GRASS, 0, pl };
                } break; 
                // Road top
                case 1: {
                    return (TileDrawInfo){ SPRITE_ROAD, 0, 0 };
                } break;
                // Road bottom
                case 2: {
                    return (TileDrawInfo){ SPRITE_ROAD, 1, 0 };
                } break;
                // Road whiteline top
                case 3: {
                    return (TileDrawInfo){ SPRITE_ROAD, 2, 0 };
                } break;
                // Road whiteline bottom
                case 4: {
                    return (TileDrawInfo){ SPRITE_ROAD, 3, 0 };
                } break;
                default: return (TileDrawInfo){ SPRITE_MAN, 0, 0 };
            }
        } break;
        case TILE_TYPE_WATER:  {
            return (TileDrawInfo){ SPRITE_WATER, 0, 0 };
        } break;
        default: return (TileDrawInfo){ SPRITE_MAN, 0, 0 };
    }
    panic();
}

u64 flag_sprite_from_tile(Tile tile) {
    if(tile.flags & TILE_FLAG_CLIFF) {
        return SPRITE_CLIFF;
    }
    return -1;
}

void draw_level_tiles(Game* game, Level* level, DrawList* draw_list, v2 pos_offset) {
    LevelPacket pk = make_level_packet(game, level);
	draw_clear_color(draw_list, v4_new(0.0f, 0.0f, 0.0f, 1.0f));
    for(i32 i = 0; i < 64; i++) {
        Tile tile = level->tiles[i];
        TileDrawInfo info = tile_draw_info(tile, i);
        v2 draw_pos = v2_add(pixel_pos_from_index(i), pos_offset);
        draw_sprite(draw_list, info.sprite, info.frame, draw_pos, info.palette);

        bool tile_cliff = (tile.flags & TILE_FLAG_CLIFF);
        if(tile_cliff) {
            i32 cliff_frame = 0;
            i32 cap_frame = -1; 
            if(tile_cliff) {
                cliff_frame = tile_cliff_frame(pos_from_index(i), &pk, &cap_frame);
            }
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
        draw_sprite_animated_frame_range(draw_list, car->sprite_handle, anim_t, draw_pos, 0, 1, car->palette_swap);
    }

    // Buttons
    for(i32 i = 0; i < state->buttons_len; i++) {
        Button* button = &state->buttons[i];
        v2 draw_pos = v2_add(v2_scale(v2_from_iv2(button->pos), 8.0), pos_offset);
        button->transition_t += dt / BUTTON_PRESS_TIME;
        if(button->transition_t > 1.0) {
            button->transition_t = 0.99;
        }
        f32 anim_t = button->transition_t;
        switch(button->state) {
            case BUTTON_OFF: {
                anim_t = 1.0 - button->transition_t;
            } break;
            case BUTTON_ON: {} break;
            default: panic();
        }
        draw_sprite_animated(draw_list, SPRITE_BUTTON, anim_t, draw_pos, 0);
    }

    // Gates
    for(i32 i = 0; i < state->gates_len; i++) {
        Gate* gate = &state->gates[i];
        v2 draw_pos = v2_add(v2_scale(v2_from_iv2(gate->pos), 8.0), pos_offset);
        gate->transition_t += dt / GATE_OPEN_TIME;
        if(gate->transition_t >= 1.0) {
            gate->transition_t = 0.99;
        }
        if(gate->transition_t <= 0.0) {
            gate->transition_t = 0.01;
        }
        switch(gate->state) {
            case GATE_CLOSED: {
                draw_sprite_animated_frame_range(draw_list, SPRITE_GATE, gate->transition_t, draw_pos, 4, 7, 0);
            } break;
            case GATE_OPEN: {
                draw_sprite_animated_frame_range(draw_list, SPRITE_GATE, gate->transition_t, draw_pos, 0, 4, 0);
            } break;
            default: panic();
        }
    }

    // Egg
    if(state->egg_exists) {
        v2 egg_draw_pos = v2_scale(v2_from_iv2(state->egg_pos), 8.0);
        egg_draw_pos = v2_add(egg_draw_pos, pos_offset);
        EggState egg_state = state->egg_states[state->egg_index];
        switch(egg_state) {
            case EGG_UNBROKEN: {
                draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 0, egg_draw_pos, 0);
            } break;
            case EGG_BREAKING: {
                draw_sprite_animated_frame_range(draw_list, SPRITE_DUCK_EXPLODE, state->egg_t, egg_draw_pos, 0, 5, 0);
            } break;
            case EGG_BROKEN: {
                draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 5, egg_draw_pos, 0);
            } break;
            case EGG_COLLECTED: {
                draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 6, egg_draw_pos, 0);
            } break;
            default: panic();
        }
    }

    // Signs
    for(i32 i = 0; i < state->signs_len; i++) {
        Sign* sign = &state->signs[i];
        v2 sign_draw_pos = v2_scale(v2_from_iv2(sign->pos), 8.0);
        sign_draw_pos = v2_add(sign_draw_pos, pos_offset);
        draw_sprite(draw_list, sign->sprite, 0, sign_draw_pos, 0);
    }

    // Marchers in reverse order.
    for(i32 i = state->marchers_len - 1; i >= 0; i--) {
        Entity* entity = &state->marchers[i];
        v2 draw_pos = entity_draw_pos(draw_list, entity, dt, 0, pos_offset);
        f32 t = (game->time + entity->anim_offset_t);
        // marcher
        draw_sprite_animated(draw_list, entity->sprite_handle, t * 0.5f, draw_pos, 0);
    }
}

