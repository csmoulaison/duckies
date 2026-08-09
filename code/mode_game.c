iv2 debug_circle_pos   = {};
iv2 debug_circle_pos_2 = {};

void update_marcher_with_platforms(LevelState* state, Entity* marcher, Entity* marcher_follower, bool is_hannah) {
    // RELEASE: remove this loop, just to make sure platforms aren't in same
    // spot.
    for(i32 i = 0; i < state->platforms_len; i++) {
        Entity* a = &state->platforms[i];
        for(i32 j = i + 1; j < state->platforms_len; j++) {
            Entity* b = &state->platforms[j];
            if(iv2_eq(a->pos_prev, b->pos_prev)) {
                printf("platforms %d an %d share position of %d, %d\n", i, j, a->pos_prev);
                //panic();
            }
        }
    }

    bool should_move_marcher = false;
    for(i32 i = 0; i < state->platforms_len; i++) {
        Entity* platform = &state->platforms[i];
        bool marcher_h_axis  = (marcher->move_this_cycle  == MOVE_LEFT || marcher->move_this_cycle  == MOVE_RIGHT);
        bool platform_h_axis = (platform->move_this_cycle == MOVE_LEFT || platform->move_this_cycle == MOVE_RIGHT);
        bool same_axis       = (marcher_h_axis == platform_h_axis);

        if(iv2_eq(platform->pos_prev, marcher->pos_cur)) {
            if(marcher->move_this_cycle == MOVE_NONE) {
                should_move_marcher = true;
            } 
        } else if(iv2_eq(platform->pos_prev, marcher->pos_prev) && marcher->move_this_cycle != MOVE_NONE) {
            if(same_axis && platform->move_this_cycle != MOVE_NONE) {
                should_move_marcher = true;
            }
        }

        if(is_hannah && marcher->move_this_cycle == MOVE_NONE) {
            if(iv2_eq(platform->pos_prev, marcher->pos_cur)) {
                entity_move(marcher, platform->move_this_cycle);
            }
        }


        if(should_move_marcher) {
            if(is_hannah) {
                iv2 original_delta = iv2_sub(state->hannah_pos_lead_prev, marcher->pos_prev);
                if(marcher->move_this_cycle == platform->move_this_cycle) {
                    if(marcher_follower != NULL) {
                        //iv2 pos_after_move = pos_after_direction(marcher, platform->move_this_cycle);
                        if(iv2_eq(marcher->pos_cur, marcher_follower->pos_cur)) {
                            //entity_move(marcher, platform->move_this_cycle);
                            marcher->pos_lead = iv2_add(marcher->pos_cur, original_delta);
                        }
                    }
                }
            } else {
                iv2 original_delta = iv2_sub(marcher->pos_lead, marcher->pos_cur);
                if(marcher_follower != NULL) {
                    iv2 new_pos = pos_after_direction(marcher, platform->move_this_cycle);
                    if(iv2_eq(new_pos, marcher_follower->pos_cur)) {
                        entity_move(marcher, platform->move_this_cycle);
                        marcher->pos_lead = iv2_add(marcher->pos_cur, original_delta);
                    }
                }
            }
            return;
        }
    }
}

void mode_game_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
    Level* level = active_game_level(game);

    i32 switch_to_level = -1;
    iv2 level_switch_offset = iv2_new(0, 0);

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
	bool level_reset = false;
    if(game->half_cycle_this_frame) {
        for(i32 i = 0; i < state->marchers_len; i++) { 
            Entity* entity = &state->marchers[i];
            entity->move_this_cycle = MOVE_NONE;

            // Check death if inside bounds
            if(entity->pos_cur.x < 0 || entity->pos_cur.x > 7 || entity->pos_cur.y < 0 || entity->pos_cur.y > 7) {
                continue;
            }
            
            for(i32 j = i + 1; j < state->marchers_len; j++) {
                Entity* other = &state->marchers[j];
                if(iv2_eq(other->pos_cur, entity->pos_cur)) {
                    level_reset = true;
                }
            }

            if(i > 0) {
                Entity* leader = &state->marchers[i - 1];
                if(iv2_distance(leader->pos_cur, entity->pos_cur) != 1.0) {
                    level_reset = true;
                }
            }
             
            if(!pos_safe(game, entity->pos_cur)) {
                level_reset = true;
            }
        }

        if(hannah->pos_cur.y > 7 && level->exit_up != 0) {
            switch_to_level = level->exit_up;
            level_switch_offset = iv2_new(0, -1);
        }
        if(hannah->pos_cur.x < 0 && level->exit_left != 0) {
            switch_to_level = level->exit_left;
            level_switch_offset = iv2_new(1, 0);
        }
        if(hannah->pos_cur.y < 0 && level->exit_down != 0) {
            switch_to_level = level->exit_down;
            level_switch_offset = iv2_new(0, 1);
        }
        if(hannah->pos_cur.x > 7 && level->exit_right != 0) {
            switch_to_level = level->exit_right;
            level_switch_offset = iv2_new(-1, 0);
        }
    }
	if(level_reset || input_button_pressed(game->input_buttons[BUTTON_RESET])) {
        reset_level(game);
        update_visual_state(game, draw_list, v2_zero(), dt);
        return;
	}

    if(game->new_cycle_this_frame) {
        // TODO: footstep puff fx

        // Reset previous visible position
        for(i32 i = 0; i < state->marchers_len; i++) {
            Entity* marcher = &state->marchers[i];
            state->marchers[i].pos_prev_visible = v2_from_iv2(state->marchers[i].pos_cur);
        }

        // Hannah moves if applicable
        if(state->input_move != MOVE_NONE) {
            state->hannah_pos_lead_prev = hannah->pos_lead;
            entity_move(hannah, state->input_move);
            state->hannah_manual_moved_this_cycle = true;
        } else {
            state->hannah_manual_moved_this_cycle = false;
        }
        Entity* hannah_follower = NULL;
        if(ducks_len(state) > 0) {
            hannah_follower = &state->ducks[0];
        }
        update_marcher_with_platforms(state, hannah, hannah_follower, true);
        state->input_move = MOVE_NONE;

        // Smart(ish) ducks follow the leader
        // Ducks follow best path by the following:
        // 
        // 1. Do any of my moves put me at my leader's pos_lead? Do that.
        // 
        // 3. If not, do any of my moves put me one square away from my leader's
        //    pos_cur? Do that.
        //    
        // 2. If not, I am going to lose next turn, but I go in whichever
        //    direction puts me closest to the leader's pos_cur.
        //    
        for(i32 i = 0; i < ducks_len(state); i++) {
            Entity* duck   = &state->ducks[i];
            Entity* leader = &state->ducks[i - 1];
            Entity* follower = NULL;
            if(i < ducks_len(state) - 1) {
                follower = &state->ducks[i + 1];
            }

            // Simulate all possible moves
            MoveDirection best_move = -1;
            Entity sim_ducks[5];
            for(MoveDirection move = 0; move < 5; move++) {
                Entity* sim_duck = &sim_ducks[move];
                *sim_duck = *duck;
                if(move != MOVE_NONE) {
                    entity_move(sim_duck, move);
                }
                update_marcher_with_platforms(state, sim_duck, follower, false);

                if(iv2_eq(leader->pos_lead, sim_duck->pos_cur)) {
                    best_move = move;
                    break;
                }
            }

            if(i == 0) {
                debug_circle_pos = leader->pos_lead;
            }
            f32 best_losing_dist = 10000.0;
            if(best_move == -1) {
                for(MoveDirection move = 0; move < 5; move++) {
                    Entity* sim_duck = &sim_ducks[move];
                    f32 dist = iv2_distance(leader->pos_lead, sim_duck->pos_cur);
                    if(dist == 1.0) {
                        best_move = move;
                        break;
                    }
                    if(dist < best_losing_dist) {
                        best_losing_dist = dist;
                        best_move = move;
                    }
                }
            }
            entity_move(duck, best_move);
            if(duck->move_this_cycle == MOVE_LEFT)  duck->sprite_handle = SPRITE_DUCK_LEFT;
            if(duck->move_this_cycle == MOVE_RIGHT) duck->sprite_handle = SPRITE_DUCK_RIGHT;
            update_marcher_with_platforms(state, duck, follower, false);
        }
    }

    // draw
    update_visual_state(game, draw_list, v2_zero(), dt);

    // Debug circle
    //debug_circle_pos = state->platforms[0].pos_cur;
    //debug_circle_pos = hannah->pos_lead;
    //draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, (game->time * 2.0), v2_scale(v2_from_iv2(debug_circle_pos), 8.0f), 0);
    //draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, ((game->time + 1.0) * 2.0f), v2_scale(v2_from_iv2(debug_circle_pos_2), 8.0f), 0);

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_EDITOR;
	}

    if(switch_to_level != -1) {
        printf("switch to level %d\n", switch_to_level);
        state->level_index_prev = state->level_index;
        state->level_index = switch_to_level;
        state->level_prev_offset_pos = v2_scale(v2_from_iv2(level_switch_offset), 64.0);
        game->transition_t = 0.0;
        game->mode = MODE_LEVEL_SWITCH;

        for(i32 i = 0; i < state->marchers_len; i++) {
            entity_offset_teleport(&state->marchers[i], iv2_scale(level_switch_offset, 8));
        }
    }

}
