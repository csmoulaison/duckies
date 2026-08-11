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
    	// TODO: if duck (not hannah) is in impassable tile (gate opened on them
    	// for example), level rest.
        for(i32 i = 0; i < state->marchers_len; i++) { 
            Entity* entity = &state->marchers[i];
            entity->move_this_cycle = MOVE_NONE;

            // Only check death if inside bounds
            if(entity->pos_cur.x < 0 || entity->pos_cur.x > 7 || entity->pos_cur.y < 0 || entity->pos_cur.y > 7) {
                continue;
            }
            
            for(i32 j = i + 1; j < state->marchers_len; j++) {
                Entity* other = &state->marchers[j];
                if(iv2_eq(other->pos_cur, entity->pos_cur)) {
                    level_reset = true;
                }
            }

            for(i32 j = 0; j < state->cars_len; j++) {
                Entity* car = &state->cars[j];
                for(i32 x = 0; x < car->car_width; x++) {
                    iv2 segment = iv2_new(car->pos_cur.x + x, car->pos_cur.y);
                    if(iv2_eq(entity->pos_cur, segment)) {
                        level_reset = true;
                    }
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

        for(i32 i = 0; i < state->buttons_len; i++) {
            Button* button = &state->buttons[i];
            ButtonState state_prev = button->state;
            button->state = BUTTON_OFF;
            for(i32 j = 0; j < state->marchers_len; j++) {
                Entity* marcher = &state->marchers[j];
                if(iv2_eq(marcher->pos_cur, button->pos)) {
                    button->state = BUTTON_ON;
                }
            }
            if(button->state != state_prev) {
                button->transition_t = 0.0;
            }
        }

        for(i32 i = 0; i < state->gates_len; i++) {
            Gate* gate = &state->gates[i];

            GateState state_prev = gate->state;
            gate->state = GATE_OPEN;
            for(i32 j = 0; j < gate->trigger.buttons.len; j++) {
                if(gate->permagate_index != -1
                && state->permagates_open[gate->permagate_index] == true) {
                    gate->state = GATE_OPEN;
                    break;
                }
                Button* button = &state->buttons[gate->trigger.buttons.indices[j]];
                if(button->state == BUTTON_OFF) {
                    gate->state = GATE_CLOSED;
                    break;
                }
            }
            if(gate->state != state_prev) {
                gate->transition_t = 0.0;
            }
            if(gate->permagate_index != -1 
            && gate->state == GATE_OPEN) {
                state->permagates_open[gate->permagate_index] = true;
            }
        }
    }

	if((level_reset && !game->god_mode) || input_button_pressed(game->input_buttons[BUTTON_RESET])) {
        reset_level(game);
        update_visual_state(game, draw_list, v2_zero(), dt);
        return;
	}

    if(game->new_cycle_this_frame) {
        // Reset previous visible position
        for(i32 i = 0; i < state->marchers_len; i++) {
            Entity* marcher = &state->marchers[i];
            state->marchers[i].pos_prev_visible = v2_from_iv2(state->marchers[i].pos_cur);
        }

        // Hannah moves if applicable
        state->hannah_manual_moved_this_cycle = false;
        if(state->input_move != MOVE_NONE) {
            iv2 move_pos = pos_after_direction(hannah, state->input_move);
            bool backtracking = false;
            if(pos_in_bounds(move_pos) && ducks_len(state) > 0 && iv2_eq(move_pos, state->ducks[0].pos_cur)) {
                backtracking = true;
            }
            if(game->god_mode || (!backtracking && pos_passable(game, move_pos))) {
                state->hannah_manual_moved_this_cycle = true;

                Sign* sign = NULL;
                for(i32 i = 0; i < state->signs_len; i++) {
                    if(iv2_eq(move_pos, state->signs[i].pos)) {
                        sign = &state->signs[i];
                        break;
                    }
                }

                if(state->egg_exists && state->egg_states[state->egg_index] == EGG_UNBROKEN && iv2_eq(move_pos, state->egg_pos)) {
                    state->egg_states[state->egg_index] = EGG_BREAKING;
                    game->mode = MODE_EGG_EXPLODE;
                    state->egg_t = 0.0;
                } else if(sign != NULL) {
                    sign_talk(game, sign);
                } else {
                    entity_move(hannah, state->input_move);
                    state->hannah_manual_moved_this_cycle = true;
                }
            }
        }
        state->hannah_pos_lead_prev = hannah->pos_lead;

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

    if(input_button_pressed(game->input_buttons[BUTTON_GOD])) {
        game->god_mode = !game->god_mode;
    }
    if(game->god_mode) {
        draw_simple_text(draw_list, string_const("GOD"), v2_new(27, 58), (i32)(game->time * 2.0) % 2);
    }
    if(input_button_pressed(game->input_buttons[BUTTON_LEAVE])) debug_timescale *= 2.0;
    if(input_button_pressed(game->input_buttons[BUTTON_BECKON])) debug_timescale *= 0.5;
    if(within_epsilon(debug_timescale, 1.0, 0.1)) {
        debug_timescale = 1.0;
    } else {
        char time_buf[16];
        String time_str = string_init(time_buf, 16);
        string_print_int(&time_str, (i32)debug_timescale);
        string_cat(&time_str, string_const("X"));
        draw_simple_text(draw_list, time_str, v2_new(29, 53), 0);
    }

    // Debug circle
    //debug_circle_pos = state->platforms[0].pos_cur;
    debug_circle_pos = hannah->pos_lead;
    draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, (game->time * 2.0), v2_scale(v2_from_iv2(debug_circle_pos), 8.0f), 0);
    debug_circle_pos = state->hannah_pos_lead_prev;
    draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, (game->time * 4.0), v2_scale(v2_from_iv2(debug_circle_pos), 8.0f), 0);
    //draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, ((game->time + 1.0) * 2.0f), v2_scale(v2_from_iv2(debug_circle_pos_2), 8.0f), 0);

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_EDITOR;
	}

    if(switch_to_level != -1) {
        // TODO: Prevent ducks from previous level from leading to endless
        // death. Always get them out of the way for switching levels.

        printf("switch to level %d\n", switch_to_level);
        state->level_index_prev = state->level_index;
        state->level_index = switch_to_level;
        state->level_prev_offset_pos = v2_scale(v2_from_iv2(level_switch_offset), 64.0);
        game->transition_t = 0.0;
        game->mode = MODE_LEVEL_SWITCH;

        for(i32 i = 0; i < state->marchers_len; i++) {
            entity_offset_teleport(&state->marchers[i], iv2_scale(level_switch_offset, 8));
        }

        for(i32 i = 0; i < SIGNS_MAX; i++) {
            state->signs[i].talk_count = 0;
        }
    }

}
