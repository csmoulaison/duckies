iv2 debug_circle_pos   = {};
iv2 debug_circle_pos_2 = {};

void update_marcher_with_moving_platforms(LevelState* state, Entity* marcher, Entity* marcher_follower) {
    // RELEASE: remove this loop, just to make sure platforms aren't in same
    // spot.
    for(i32 i = 0; i < state->logic_entities_len; i++) {
        Entity* a = &state->logic_entities[i];
        if(a->logic_type != LOGIC_MOVING_PLATFORM) continue;
        for(i32 j = i + 1; j < state->logic_entities_len; j++) {
            Entity* b = &state->logic_entities[j];
            if(b->logic_type != LOGIC_MOVING_PLATFORM) continue;
            if(iv2_eq(a->pos_prev, b->pos_prev)) {
                printf("platforms %d an %d share position of %d, %d\n", i, j, a->pos_prev);
                panic();
            }
        }
    }

    bool should_move_marcher = false;
    for(i32 i = 0; i < state->logic_entities_len; i++) {
        Entity* platform = &state->logic_entities[i];
        if(platform->logic_type != LOGIC_MOVING_PLATFORM) {
            continue;
        }

        if(iv2_eq(platform->pos_prev, marcher->pos_cur)) {
            if(marcher->move_this_cycle == MOVE_NONE) {
                should_move_marcher = true;
            } 
        } else if(iv2_eq(platform->pos_prev, marcher->pos_prev) && marcher->move_this_cycle != MOVE_NONE) {
            bool marcher_h_axis  = (marcher->move_this_cycle  == MOVE_LEFT || marcher->move_this_cycle  == MOVE_RIGHT);
            bool platform_h_axis = (platform->move_this_cycle == MOVE_LEFT || platform->move_this_cycle == MOVE_RIGHT);
            if(marcher_h_axis == platform_h_axis && platform->move_this_cycle != MOVE_NONE) {
                should_move_marcher = true;
            }
        }
        if(should_move_marcher) {
            iv2 original_delta = iv2_sub(marcher->pos_lead, marcher->pos_cur);
            entity_move(marcher, platform->move_this_cycle);
            if(marcher_follower != NULL) {
                if(iv2_eq(marcher->pos_cur, marcher_follower->pos_cur)) {
                    marcher->pos_lead = iv2_add(marcher->pos_cur, original_delta);
                }
            }
            return;
        }
    }
}

void mode_game_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;

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
    }
	if(level_reset || input_button_pressed(game->input_buttons[BUTTON_RESET])) {
        reset_level(game);
        update_visual_state(game, draw_list, dt);
        return;
	}

    if(game->new_cycle_this_frame) {
        // Reset previous visible position
        for(i32 i = 0; i < state->marchers_len; i++) {
            Entity* marcher = &state->marchers[i];
            state->marchers[i].pos_prev_visible = v2_from_iv2(state->marchers[i].pos_cur);
        }

        // Hannah moves if applicable
        if(state->input_move != MOVE_NONE) {
            entity_move(hannah, state->input_move);
        }
        Entity* hannah_follower = NULL;
        if(ducks_len(state) > 0) {
            hannah_follower = &state->ducks[0];
        }
        update_marcher_with_moving_platforms(state, hannah, hannah_follower);
        state->input_move = MOVE_NONE;

        // Smart(ish) ducks follow the leader
        for(i32 i = 0; i < ducks_len(state); i++) {
            Entity* duck   = &state->ducks[i];
            Entity* leader = &state->ducks[i - 1];
            Entity* follower = NULL;
            if(i < ducks_len(state) - 1) {
                follower = &state->ducks[i + 1];
            }
            MoveDirection follow_move = direction_from_target(duck, leader->pos_lead);

            // Simulate the moving of platforms on both staying put and
            // following the leader, choosing the one which places the duck
            // closer to the leader's previous position.
            Entity follow_sim_duck = *duck;
            entity_move(&follow_sim_duck, follow_move);
            update_marcher_with_moving_platforms(state, &follow_sim_duck, follower);

            Entity stay_sim_duck = *duck;
            update_marcher_with_moving_platforms(state, &stay_sim_duck, follower);

            // The distance check should work for choosing not to follow leader
            // because we are already next to him and he hasn't moved, but if
            // for some reason there's some hole in this logic, we just need to
            // check for it explicitly.
            if(iv2_distance(follow_sim_duck.pos_cur, leader->pos_prev) < iv2_distance(stay_sim_duck.pos_cur, leader->pos_prev)) {
                entity_move(duck, follow_move);
            }
            if(duck->move_this_cycle == MOVE_LEFT)  duck->sprite_handle = SPRITE_DUCK_LEFT;
            if(duck->move_this_cycle == MOVE_RIGHT) duck->sprite_handle = SPRITE_DUCK_RIGHT;
            update_marcher_with_moving_platforms(state, duck, follower);
        }
    }

    // draw
    update_visual_state(game, draw_list, dt);

    // Debug circle
    debug_circle_pos = state->logic_entities[0].pos_cur;
    draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, (game->time * 2.0), v2_scale(v2_from_iv2(debug_circle_pos), 8.0f), 0);
    //draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, ((game->time + 1.0) * 2.0f), v2_scale(v2_from_iv2(debug_circle_pos_2), 8.0f), 0);

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_EDITOR;
	}

}
