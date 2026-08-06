void mode_game_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
	if(input_button_pressed(game->input_buttons[BUTTON_RESET])) {
        reset_level(game);
	}

	// leave/beckon
	if(input_button_pressed(game->input_buttons[BUTTON_LEAVE])) {
    	if(state->active_ducks_len < state->all_ducks_len) {
        	state->active_ducks_len++;
    	}
	}
	if(input_button_pressed(game->input_buttons[BUTTON_BECKON])) {
    	if(state->active_ducks_len > 0) {
        	state->active_ducks_len--;
    	}
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
        for(i32 i = 0; i < marchers_len(state); i++) { 
            Entity* entity = &state->marchers[i];
            if(!pos_safe(game, entity->pos_cur)) {
                reset_level(game);
            }
        }
    } else if(game->new_cycle_this_frame) {
        // NOW: we are making it so ducks must start moving, and the chain head does every time,
        // and they just copy them moves.
        state->move_chain_head--;
        for(i32 i = 0; i < marchers_len(state); i++) {
            state->marchers[i].pos_prev = state->marchers[i].pos_cur;
        }

        // If hannah has decided to move and she can, she ignores the platform
        if(move_passable(game, hannah, state->input_move)) {
            entity_move_direction(state, hannah, state->input_move);
        } else {
            Entity* platform = try_platform_was_at_pos(game, hannah->pos_cur);
            if(platform != NULL && entity_moved_this_cycle(platform)) {
                state->move_chain_head--;
                entity_move_position(state, hannah, platform->pos_cur);
            }
        }
        state->input_move = MOVE_NONE;

        for(i32 i = 0; i < state->all_ducks_len; i++) { 
            Entity* duck = &state->ducks[i];
            iv2 target_pos = duck->pos_cur;
            if(i < state->active_ducks_len) {
                target_pos = pos_at_relative_chain_index(state, duck->chain_index);
                Entity* leader = &state->ducks[i - 1];
                if(iv2_eq(target_pos, leader->pos_cur)) {
                    target_pos = duck->pos_cur;
                }
            } else {
                duck->chain_index++;
                if(duck->chain_index > 8) {
                    state->active_ducks_len++;
                }
            }
            entity_move_position(state, duck, target_pos);

            MoveDirection direction = direction_from_delta(duck->pos_prev, duck->pos_cur);
            if(direction == MOVE_LEFT)  duck->sprite_handle = SPRITE_DUCK_LEFT;
            if(direction == MOVE_RIGHT) duck->sprite_handle = SPRITE_DUCK_RIGHT;
        }
    }

    // draw
    update_visual_state(game, draw_list, dt);

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_EDITOR;
	}

}
