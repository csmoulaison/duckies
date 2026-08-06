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
        for(i32 i = 0; i < marchers_len(state); i++) {
            state->marchers[i].pos_prev = state->marchers[i].pos_cur;
        }

        // Ducks skip a step if hannah moves.

        // If hannah has decided to move and she can, she ignores the platform
        bool hannah_moved = false;
        if(state->input_move != MOVE_NONE && move_passable(game, hannah, state->input_move)) {
            state->move_chain_head--;
            entity_move_direction(state, hannah, state->input_move);
            hannah_moved = true;
        } else {
            Entity* platform = try_platform_was_at_pos(game, hannah->pos_cur);
            if(platform != NULL && entity_moved_this_cycle(platform)) {
                state->move_chain_head--;
                entity_move_position(state, hannah, platform->pos_cur);
                hannah_moved = true;
            }
        }
        state->input_move = MOVE_NONE;

        // okay simpler solution. hannah tracks the chain. ducks figure ou where
        // they are first thing.
        for(i32 i = 0; i < state->all_ducks_len; i++) { 
            Entity* duck = &state->ducks[i];
            Entity* platform = try_platform_was_at_pos(game, duck->pos_cur);
            if(platform != NULL && entity_moved_this_cycle(platform)) {
                entity_move_position(state, duck, platform->pos_cur);
            } 

            Entity* leader = &state->ducks[i - 1];
            for(i32 j = leader->chain_index; j < MOVE_CHAIN_SIZE - leader->chain_index; j++) {
                if(iv2_eq(pos_at_relative_chain_index(state, j), duck->pos_cur)) {
                    duck->chain_index = j;
                    break;
                }
            }
            assert(duck->chain_index != leader->chain_index);

            if(i < state->active_ducks_len) {
                // Now we calculate where to be in the chain. Our leader has
                // already done this, so we look to him for reference. We count
                // back from his chain index until we reach our position. That 
                // is our chain index. Then we do the stuff with the gap closing
                // and the rest.
                // Close the gap between us and the leader.
                assert(leader->chain_index < duck->chain_index);
                if(duck->chain_index - leader->chain_index > 1) {
                    duck->chain_index--;
                }
                assert(duck->chain_index != leader->chain_index);
                
                iv2 target_pos = pos_at_relative_chain_index(state, duck->chain_index);
                if(!iv2_eq(target_pos, duck->pos_cur)) {
                    entity_move_position(state, duck, target_pos);
                }
            }

            MoveDirection direction = direction_from_delta(duck->pos_prev, duck->pos_cur);
            if(direction == MOVE_LEFT)  duck->sprite_handle = SPRITE_DUCK_LEFT;
            if(direction == MOVE_RIGHT) duck->sprite_handle = SPRITE_DUCK_RIGHT;
        
            //Entity* duck = &state->ducks[i];
            //MoveDirection queued_move = MOVE_NONE;
            //if(i < state->followers_len + 1) {
            //    Entity* leader = &state->entities[i - 1];
            //    if(leader->move_queue_len > 0) {
            //        iv2 leader_move_position = leader->move_queue[0];
            //        queued_move = move_from_delta(duck->pos_cur, leader_move_position);
            //        leader->move_queue_len--;
            //        for(i32 j = 0; j < leader->move_queue_len; j++) {
            //            leader->move_queue[j] = leader->move_queue[j + 1];
            //        }
            //    }
            //}

            //if(queued_move != MOVE_NONE && duck->pull_move != queued_move) {
            //    Level* level = active_game_level(game);
            //    entity_move(duck, queued_move);
            //} else if(duck->pull_move != MOVE_NONE) {
            //    entity_move(duck, duck->pull_move);
            //}
            //if(queued_move == MOVE_LEFT)  duck->sprite_handle = SPRITE_DUCK_LEFT;
            //if(queued_move == MOVE_RIGHT) duck->sprite_handle = SPRITE_DUCK_RIGHT;
        }
    }
skipit:

    // draw
    update_visual_state(game, draw_list, dt);

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_EDITOR;
	}

}
