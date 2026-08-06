iv2 debug_circle_pos   = {};
iv2 debug_circle_pos_2 = {};

void mode_game_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
	if(input_button_pressed(game->input_buttons[BUTTON_RESET])) {
        reset_level(game);
	}

    // hannah move
    Entity* hannah = &state->hannah;
	if(input_button_pressed(game->input_buttons[BUTTON_UP])) {
    	state->input_move = MOVE_UP;
    	hannah->sprite_handle = SPRITE_HANNAH_UP;
	}
	else if(input_button_pressed(game->input_buttons[BUTTON_LEFT])) {
    	state->input_move = MOVE_LEFT;
    	hannah->sprite_handle = SPRITE_HANNAH_LEFT;
	}
	else if(input_button_pressed(game->input_buttons[BUTTON_DOWN])) {
    	state->input_move = MOVE_DOWN;
    	hannah->sprite_handle = SPRITE_HANNAH_DOWN;
	}
	else if(input_button_pressed(game->input_buttons[BUTTON_RIGHT])) {
    	state->input_move = MOVE_RIGHT;
    	hannah->sprite_handle = SPRITE_HANNAH_RIGHT;
	}

	// used to calc the effect of the last move
    if(game->half_cycle_this_frame) {
        for(i32 i = 0; i < state->marchers_len; i++) { 
            Entity* entity = &state->marchers[i];
            if(!pos_safe(game, entity->pos_cur)) {
                reset_level(game);
            }
        }
    } else if(game->new_cycle_this_frame) {
        // Marchers reset visible previous position and get moved by platforms.
        for(i32 i = 0; i < state->marchers_len; i++) {
            Entity* marcher = &state->marchers[i];
            state->marchers[i].pos_prev_visible = v2_from_iv2(state->marchers[i].pos_cur);

            // NOW: Doing platform movement first means a dismount moves on the
            // same turn. Dismounts should not participate in the platform
            // movement. Mounts should(?).
            marcher->move_this_cycle = MOVE_NONE;
            marcher->pulled_move_this_cycle = MOVE_NONE;
            Entity* platform = try_platform_was_at_pos(game, marcher->pos_cur);
            if(platform != NULL && platform->move_this_cycle != MOVE_NONE) {
                entity_move(state, marcher, platform->move_this_cycle);
                marcher->pulled_move_this_cycle = platform->move_this_cycle;
            }
        }

        // Hannah moves if applicable.
        if(state->input_move != MOVE_NONE && move_passable(game, hannah, state->input_move)) {
            entity_move(state, hannah, state->input_move);
        }
        state->input_move = MOVE_NONE;

        // Ducks follow the leader
        for(i32 i = 0; i < ducks_len(state); i++) { 
            Entity* duck = &state->ducks[i];
            Entity* leader = &state->ducks[i - 1];
            if(leader->move_this_cycle != MOVE_NONE) {
                // NOW: Only problem is dismounting. Ducks get a little confused
                // and don't take the step when a space is made.
                iv2 target = leader->pos_prev;
                if(duck->pulled_move_this_cycle != MOVE_NONE 
                && leader->pulled_move_this_cycle == duck->pulled_move_this_cycle) {
                    iv2 delta = delta_from_direction(duck->pulled_move_this_cycle);
                    //printf("delta %d %d\n", delta.x, delta.y);
                    if(iv2_eq(delta, iv2_sub(duck->pos_cur, leader->pos_cur))) {
                        target = iv2_add(target, iv2_scale(delta, 2));
                    }
                }
                debug_circle_pos = target;
                if(!iv2_eq(target, leader->pos_cur)) {
                    // NOW: direction_from_target gets confused on diagonals. Diagonals sholdn't happen.
                    // They happen here because of dismounting from a moving platform.
                    MoveDirection move = direction_from_target(duck, target);
                    //assert(move != MOVE_NONE);
                    entity_move(state, duck, move);
                    if(move == MOVE_LEFT)  duck->sprite_handle = SPRITE_DUCK_LEFT;
                    if(move == MOVE_RIGHT) duck->sprite_handle = SPRITE_DUCK_RIGHT;
                } else {
                    printf("couldnt move\n");
                }
            }
        }
    }

    // draw
    update_visual_state(game, draw_list, dt);

    // Debug circle
    draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, (game->time * 2.0), v2_scale(v2_from_iv2(debug_circle_pos), 8.0f), 0);
    draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, ((game->time + 1.0) * 2.0f), v2_scale(v2_from_iv2(debug_circle_pos_2), 8.0f), 0);

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_EDITOR;
	}

}
