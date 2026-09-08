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
                // Mountain pass colors
                case 5: {
                    u32 off_i = i + (i / 8);
                    i32 pl = 5;
                    if(off_i % 2 == 1) pl = 6;
                    return (TileDrawInfo){ SPRITE_GRASS, 0, pl };
                } break;
                // Bridge left
                case 6: {
                    return (TileDrawInfo){ SPRITE_BRIDGE, 0, 0 };
                } break;
                // Bridge right
                case 7: {
                    return (TileDrawInfo){ SPRITE_BRIDGE, 1, 0 };
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
            i32 palette = 0;
            if(tile.subtype == 5) {
                palette = 5;
            }
            i32 cliff_frame = 0;
            i32 cap_frame = -1; 
            if(tile_cliff) {
                cliff_frame = tile_cliff_frame(pos_from_index(i), &pk, &cap_frame);
            }
            draw_sprite(draw_list, flag_sprite_from_tile(tile), cliff_frame, draw_pos, palette);
            if(cap_frame != -1) {
                draw_sprite_layer(draw_list, SPRITE_CLIFF, cap_frame, v2_add(draw_pos, v2_new(0.0, 8.0)), palette, 1);
            }
        }
    }
}

void draw_muffin_minigame(Game* game, DrawList* draw_list, v2 pos_offset, f32 dt) {
    LevelState* state = &game->state;
    MuffinGame* minigame = &state->muffin_game;
    for(i32 i = 0; i < 9; i++) {
        Muffin* muffin = &minigame->muffins[i];
        iv2 pos = muffin_pos_from_index(i);
        v2 draw_pos = v2_add(v2_scale(v2_from_iv2(pos), 8.0), pos_offset);
        switch(muffin->state) {
            case MUFFIN_DOWN: {
                draw_sprite(draw_list, SPRITE_MUFFIN, 2, draw_pos, 0);
            } break;
            case MUFFIN_WARN: {
                draw_sprite_animated_frame_range(draw_list, SPRITE_MUFFIN, game->time, draw_pos, 3, 4, 0);
            } break;
            case MUFFIN_UP: {
                if(minigame->mode == MUFFIN_GAME_LOST) {
                    draw_sprite_animated_frame_range(draw_list, SPRITE_MUFFIN, game->time, draw_pos, 10, 13, 0);
                } else {
                    draw_sprite_animated_frame_range(draw_list, SPRITE_MUFFIN, game->time, draw_pos, 0, 1, 0);
                }
            } break;
            case MUFFIN_ASLEEP: {
                draw_sprite(draw_list, SPRITE_MUFFIN, 2, draw_pos, 0);
                draw_sprite_animated(draw_list, SPRITE_ZEEZ, game->time + (i / 2.0), v2_add(draw_pos, v2_new(0, 4)), 0);
            } break;
        }
    }
}

v2 entity_draw_pos(DrawList* draw_list, Entity* entity, f32 dt, i32 palette, v2 offset) {
    entity->pos_t += TIME_SCALE * 2.0f * dt;
    if(entity->pos_t > 1.0f) {
        entity->pos_t = 1.0f;
    }
    v2 lerped = v2_new(
        lerp(entity->pos_prev_visible.x, entity->pos_cur.x, smootherstep(entity->pos_t)) * 8.0f,
        lerp(entity->pos_prev_visible.y, entity->pos_cur.y, smootherstep(entity->pos_t)) * 8.0f);
    //lerped = v2_new(
    //    lerp(entity->pos_prev_visible.x, entity->pos_cur.x, entity->pos_t * 8.0f,
    //    lerp(entity->pos_prev_visible.y, entity->pos_cur.y, entity->pos_t * 8.0f);
    return v2_add(lerped, offset);
}

void draw_gate_t(DrawList* list, iv2 pos, f32 t, GateState gate_state, v2 pos_offset) {
    v2 draw_pos = v2_add(v2_scale(v2_from_iv2(pos), 8.0), pos_offset);
    if(t >= 1.0) {
        t = 0.99;
    }
    if(t <= 0.0) {
        t = 0.01;
    }
    switch(gate_state) {
        case GATE_CLOSED: {
            draw_sprite_animated_frame_range(list, SPRITE_GATE, t, draw_pos, 4, 7, 0);
        } break;
        case GATE_OPEN: {
            draw_sprite_animated_frame_range(list, SPRITE_GATE, t, draw_pos, 0, 4, 0);
        } break;
        default: panic();
    }
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
        if(platform->crumbler) {
            if(platform->hits_taken < 4) {
                i32 frame = platform->hits_taken - 1;
                if(frame < 0) frame = 0;
                draw_sprite(draw_list, SPRITE_CRUMBLER, frame, draw_pos, 0);
            }
            continue;
        }

        if(platform->snake_index != 0) {
            i32 sprite = SPRITE_SNAKE_BODY;
            if(platform->snake_index == 8) {
                sprite = SPRITE_SNAKE_HEAD;
            } else if(platform->snake_index == 1) {
                sprite = SPRITE_SNAKE_TAIL;
            }
            bool corner_change = (platform->pos_t > 0.5);
            if(platform->move_this_cycle != MOVE_NONE) {
                if(platform->snake_index == 8) {
                    platform->snake_frame_root = 3 * (platform->move_this_cycle - 1);
                } else if(platform->snake_index == 1) {
                    if(corner_change) {
                        MoveDirection delta_dir = direction_from_delta(platform->pos_cur, state->platforms[i + 1].pos_cur);
                        platform->snake_frame_root = 3 * (delta_dir - 1);
                    }
                } else {
                    // these are actually flipped lol but it works now so its fine
                    MoveDirection dir_from_prev = direction_from_delta(state->platforms[i + 1].pos_cur, platform->pos_cur);
                    MoveDirection dir_from_next = direction_from_delta(state->platforms[i - 1].pos_cur, platform->pos_cur);
                    if((dir_from_prev == MOVE_DOWN && dir_from_next == MOVE_UP)
                    || (dir_from_prev == MOVE_UP && dir_from_next == MOVE_DOWN)) {
                        platform->snake_frame_root = 15; // up/down
                    } else if((dir_from_prev == MOVE_LEFT && dir_from_next == MOVE_RIGHT)
                    || (dir_from_prev == MOVE_RIGHT && dir_from_next == MOVE_LEFT)) {
                        platform->snake_frame_root = 12; // left/right
                    } else if((dir_from_prev == MOVE_UP && dir_from_next == MOVE_LEFT)
                    || (dir_from_prev == MOVE_LEFT && dir_from_next == MOVE_UP)) {
                        if(corner_change) platform->snake_frame_root = 0; // bottom right corner
                    } else if((dir_from_prev == MOVE_UP && dir_from_next == MOVE_RIGHT)
                    || (dir_from_prev == MOVE_RIGHT && dir_from_next == MOVE_UP)) {
                        if(corner_change) platform->snake_frame_root = 3; // bottom left corner
                    } else if((dir_from_prev == MOVE_DOWN && dir_from_next == MOVE_LEFT)
                    || (dir_from_prev == MOVE_LEFT && dir_from_next == MOVE_DOWN)) {
                        if(corner_change) platform->snake_frame_root = 6; // top right corner
                    } else if((dir_from_prev == MOVE_DOWN && dir_from_next == MOVE_RIGHT)
                    || (dir_from_prev == MOVE_RIGHT && dir_from_next == MOVE_DOWN)) {
                        if(corner_change) platform->snake_frame_root = 9; // top left corner
                    }
                }
            }
            draw_sprite(draw_list, sprite, platform->snake_frame_root, draw_pos, 0);
            continue;
        }

        switch(platform->sink_state) {
            case PLATFORM_FLOAT: {
                // weird fuckery with transition t lol
                draw_sprite_animated_frame_range(draw_list, platform->sprite_handle, anim_t, draw_pos, 0, 1, 0);
                platform->transition_t = 0.0;
            } break;
            case PLATFORM_WARN: 
            case PLATFORM_FAKE_WARN: {
                platform->transition_t += TIME_SCALE * dt;
                if(platform->transition_t < 0.25) {
                    draw_sprite(draw_list, platform->sprite_handle, 1, draw_pos, 0);
                } else {
                    draw_sprite(draw_list, platform->sprite_handle, 2, draw_pos, 0);
                }
                if(platform->transition_t > 0.4) {
                    platform->transition_t = 0.4;
                }
            } break;
            case PLATFORM_SINK: {
                platform->transition_t += TIME_SCALE * dt;
                if(platform->transition_t < 0.65) {
                    draw_sprite(draw_list, platform->sprite_handle, 3, draw_pos, 0);
                }
            } break;
            default: panic();
        }
    }

    // Cars
    for(i32 i = 0; i < state->cars_len; i++) {
        Entity* car = &state->cars[i];
        v2 draw_pos = entity_draw_pos(draw_list, car, dt, 0, pos_offset);
        i32 frame_off = 0;
        if(car->move_this_cycle == MOVE_RIGHT) {
            frame_off = 2;
        }
        draw_sprite_animated_frame_range(draw_list, car->sprite_handle, anim_t, draw_pos, frame_off, frame_off + 1, car->palette_swap);
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
            case BUTTON_ON: {
            } break;
            default: break;
        }
        draw_sprite_animated(draw_list, SPRITE_BUTTON, anim_t, draw_pos, 0);
    }

    // Gates
    for(i32 i = 0; i < state->gates_len; i++) {
        Gate* gate = &state->gates[i];
        gate->transition_t += dt / GATE_OPEN_TIME;
        draw_gate_t(draw_list, gate->pos, gate->transition_t, gate->state, pos_offset);
    }

    // Egg
    if(state->egg_exists && !state->override_egg_draw) {
        v2 egg_draw_pos = v2_scale(v2_from_iv2(state->egg_pos), 8.0);
        egg_draw_pos = v2_add(egg_draw_pos, pos_offset);
        EggState egg_state = state->egg_states[state->egg_index];
        switch(egg_state) {
            case EGG_UNBROKEN: {
                draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 0, egg_draw_pos, 0);
            } break;
            case EGG_BREAKING: {
                i32 pl = palette_from_marcher_index(state->marchers_len);
                draw_sprite_animated_frame_range(draw_list, SPRITE_DUCK_EXPLODE, state->egg_t, egg_draw_pos, 1, 5, pl);
            } break;
            case EGG_BROKEN: {
                i32 pl = palette_from_marcher_index(state->marchers_len);
                draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 5, egg_draw_pos, pl);
            } break;
            case EGG_COLLECTED: {
                draw_sprite(draw_list, SPRITE_DUCK_EXPLODE, 6, egg_draw_pos, 0);
            } break;
            default: {
                printf("egg not in regular state! %d\n", egg_state);
            } break;
        }
    }

    // Signs
    for(i32 i = 0; i < state->signs_len; i++) {
        Sign* sign = &state->signs[i];
        v2 sign_draw_pos = v2_scale(v2_from_iv2(sign->pos), 8.0);
        sign_draw_pos = v2_add(sign_draw_pos, pos_offset);
        draw_sprite(draw_list, sign->sprite, 0, sign_draw_pos, 0);
    }

    if(state->level_index == 67) {
        draw_muffin_minigame(game, draw_list, pos_offset, dt);
    }

    // Large gate
    if(state->level_index == 35) {
        if(state->gates[0].state == GATE_OPEN) {
            state->large_gate_t += dt / 4.0;
            if(state->large_gate_t >= 1.0) {
                state->large_gate_t = 0.99;
            }
        }
        draw_sprite_animated_layer(draw_list, SPRITE_LARGE_GATE, state->large_gate_t, v2_add(v2_new(8.0, 32.0), pos_offset), 0, 0);
    }

    // Apple
    if(state->level_index == 36) {
        SnakeGame* minigame = &state->snake_game;
        if(minigame->apple_active) {
            v2 draw_pos = v2_scale(v2_from_iv2(minigame->apple_pos), 8.0);
            draw_pos.y += sin(game->time * 6.0);
            draw_sprite(draw_list, SPRITE_APPLE, 0, draw_pos, 0);
        }
    }

    // Marchers in reverse order.
    for(i32 i = state->marchers_len - 1; i >= 0; i--) {
        if(i == 0 && state->override_hannah_draw) {
            continue;
        }
        Entity* marcher = &state->marchers[i];
        v2 draw_pos = entity_draw_pos(draw_list, marcher, dt, 0, pos_offset);
        f32 t = (game->time + marcher->anim_offset_t);
        // marcher
        i32 pl = palette_from_marcher_index(i);
        if(marcher->honk_this_cycle) pl = 14;
        draw_sprite_animated(draw_list, marcher->sprite_handle, t * 0.5f, draw_pos, pl);
    }

    // Frogo
    if(state->frogo_exists) {
        Entity* frogo = &state->frogo;
        v2 draw_pos = v2_scale(v2_from_iv2(frogo->pos_cur), 8.0);

        i32 pl = 0;
        if(state->frogo_alerted && (i32)(game->time * 4.0) % 3 == 0) pl = 15;
        frogo->pos_t += TIME_SCALE * 2.0 * dt;
        if(frogo->pos_t > 1.0) {
            frogo->pos_t = 0.9;
        }
        f32 frogo_t = frogo->pos_t;

        switch(frogo->move_this_cycle) {
            case MOVE_UP: {
                frogo->sprite_handle = SPRITE_FROG_LEAP_UP;
                draw_pos.y -= 8.0;
            } break;
            case MOVE_LEFT: {
                frogo->sprite_handle = SPRITE_FROG_LEAP_LEFT;
            } break;
            case MOVE_DOWN: {
                frogo->sprite_handle = SPRITE_FROG_LEAP_DOWN;
            } break;
            case MOVE_RIGHT: {
                frogo->sprite_handle = SPRITE_FROG_LEAP_RIGHT;
                draw_pos.x -= 8.0;
            } break;
            default: {
                draw_pos = v2_scale(v2_from_iv2(frogo->pos_cur), 8.0);
                if(frogo->sprite_handle == SPRITE_FROG_LEAP_UP) draw_pos.y -= 8.0;
                if(frogo->sprite_handle == SPRITE_FROG_LEAP_RIGHT) draw_pos.x -= 8.0;
            } break;
        }
        draw_pos = v2_add(draw_pos, pos_offset);
        draw_sprite_animated(draw_list, frogo->sprite_handle, frogo_t, draw_pos, pl);
    }
}

void draw_tea_party(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
    draw_level_tiles(game, active_game_level(game), draw_list, v2_zero());
    draw_sprite_animated(draw_list, SPRITE_HANNAH_TEA, game->time, v2_new(44, 8), 0);
    for(i32 i = 0; i < ducks_len(state); i++) {
        draw_sprite_animated(draw_list, SPRITE_DUCK_TEA, game->time, v2_new(-4 + i * 8, 0), palette_from_marcher_index(i + 1));
    }
    draw_sprite_animated(draw_list, SPRITE_TROLL_TEA, game->time, v2_new(8, 16), 0);
    draw_sprite_animated(draw_list, SPRITE_FROG_TEA, game->time, v2_new(32, 24), 0);
    draw_sprite(draw_list, SPRITE_FROG_LEAP_RIGHT, 1, v2_new(16, 16), 0);
    draw_sprite_animated(draw_list, SPRITE_MUFFIN_TEA, game->time, v2_new(48, 24), 0);
    game->music_override_state = MUSIC_OVERRIDE_WINDY;
}
