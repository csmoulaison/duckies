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
                //printf("platforms %d an %d share position of %d, %d\n", i, j, a->pos_prev);
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

void frogo_move(Game* game) {
    LevelState* state = &game->state;
    Entity* frogo = &state->frogo;
    if(state->frogo_alerted_just_now) {
        state->frogo_alerted_just_now = false;
        return;
    }
    if(state->frogo_exists && state->frogo_alerted) {
        if(pos_passable(game, pos_after_direction(frogo, state->frogo_alert_direction))) {
            entity_move(frogo, state->frogo_alert_direction);
        } else {
            state->frogo_alert_direction = MOVE_NONE;
        }
    }
}

void mode_game_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    LevelState* state = &game->state;
    Level* level = active_game_level(game);
    Entity* hannah = &state->marchers[0];

    i32 switch_to_level = -1;
    iv2 level_switch_offset = iv2_new(0, 0);
    // NOW: Level reset is bringing back first move of the level if it's done at
    // the first opportunity on level load. 
    update_input_move(game);

    // Half cycle logic
	bool level_reset = false;
    if(game->half_cycle_this_frame) {
        if(state->level_index == 35 && state->marchers[0].pos_cur.y > 3) {
            game->mode = MODE_GATE_TO_TEA;
            game->transition_t = 0.0;
        }
        
        // Marchers
    	// TODO: if duck (not hannah) is in impassable tile (gate opened on them
    	// for example), level rest.
        for(i32 i = 0; i < state->marchers_len; i++) { 
            Entity* entity = &state->marchers[i];
            entity->move_this_cycle = MOVE_NONE;

            // Only check death if inside bounds
            if(entity->pos_cur.x < 0 || entity->pos_cur.x > 7 || entity->pos_cur.y < 0 || entity->pos_cur.y > 7) {
                continue;
            }

            // Check reset conditions
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

        frogo_move(game);
        if(state->frogo_exists) {
            for(i32 i = 0; i < state->marchers_len; i++) {
                if(iv2_eq(state->marchers[i].pos_cur, state->frogo.pos_cur)) {
                    level_reset = true;
                    break;
                }
            }   
        }

        // Level switching
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

        // Check queued time event
        TimeEvent* queued_event = &state->queued_time_event;
        switch(queued_event->type) {
            case TIME_EVENT_BUTTON_GATE: {
                bool triggered = true;
                for(i32 i = 0; i < queued_event->button_gate.buttons_len; i++) {
                    Button* button = &state->buttons[queued_event->button_gate.button_indices[i]];
                    if(button->state == BUTTON_OFF) {
                        triggered = false;
                        break;
                    }
                }
                if(triggered) {
                    state->active_time_event = *queued_event;
                }
            } break;
            default: break;
        }

        // Update active event
        TimeEvent* active_event = &state->active_time_event;
        if(active_event->type != TIME_EVENT_NONE) {
            if(active_event->cycles < 1) {
                switch(active_event->type) {
                    case TIME_EVENT_BUTTON_GATE: {
                        state->permagates_open[active_event->button_gate.permagate_index] = false;
                    } break;
                    default: panic();
                }
                active_event->type = TIME_EVENT_NONE;
            } else {
                active_event->cycles--;
            }
        }

        // Gates
        for(i32 i = 0; i < state->gates_len; i++) {
            Gate* gate = &state->gates[i];
            GateState state_prev = gate->state;

            switch(gate->trigger_type) {
                case TRIGGER_BUTTONS: {
                    gate->state = GATE_OPEN;
                    for(i32 j = 0; j < gate->trigger.buttons.len; j++) {
                        if(gate->permagate_index != -1 && state->permagates_open[gate->permagate_index] == true) {
                            gate->state = GATE_OPEN;
                            break;
                        }
                        Button* button = &state->buttons[gate->trigger.buttons.indices[j]];
                        if(button->state == BUTTON_OFF) {
                            gate->state = GATE_CLOSED;
                            break;
                        }
                    }
                } break;
                case TRIGGER_REMOTE: {
                    if(gate->permagate_index != -1 && state->permagates_open[gate->permagate_index] == true) {
                        gate->state = GATE_OPEN;
                    } else {
                        gate->state = GATE_CLOSED;
                    }
                } break;
                default: panic();
            }
            if(gate->state != state_prev) {
                gate->transition_t = 0.0;
                if(state->level_index == 34 && gate->state == GATE_OPEN) {
                    start_cutscene(game, CUT_RIVER_GATE);
                }
            }
            if(gate->permagate_index != -1 && gate->state == GATE_OPEN) {
                state->permagates_open[gate->permagate_index] = true;
            }
        }
    }

    if(state->muffin_game.mode == MUFFIN_GAME_LOST) {
        level_reset = true;
    }

    // Level reset logic
	if((level_reset && !game->god_mode) || input_button_pressed(game->input_buttons[BUTTON_RESET])) {
        reset_level(game);
        update_visual_state(game, draw_list, v2_zero(), dt);
        return;
	}

	// Trigger dialogue on muffin sprout area
	if(state->level_index == 67 && state->muffin_game.mode == MUFFIN_GAME_INACTIVE) {
        Msg msg[6] = {
            new_msg(MSG_MUFFIN, string_const("SING THE SPROUTS TO SLEEP!")),
            new_msg(MSG_MUFFIN, string_const("IF LEFT AWAKE FOR TOO LONG, THEY'LL CAUSE A FUSS!")),
            new_msg(MSG_MUFFIN, string_const("TO HEAR YOU, THE HONK MUST BE DIRECTLY NEXT TO THEM.")),
            new_msg(MSG_MUFFIN, string_const("READY...")),
            new_msg(MSG_MUFFIN, string_const("SET...")),
            new_msg(MSG_MUFFIN, string_const("GO!!"))
        };
        state->muffin_game.mode = MUFFIN_GAME_ACTIVE;
        start_msg(game, msg, 6, MODE_GAME);
	}

    if(game->new_cycle_this_frame) {
        // Reset previous visible position
        for(i32 i = 0; i < state->marchers_len; i++) {
            Entity* marcher = &state->marchers[i];
            state->marchers[i].pos_prev_visible = v2_from_iv2(state->marchers[i].pos_cur);
        }

        // Iterate backwards to propogate honks
        for(i32 i = state->marchers_len - 1; i >= 0; i--) {
            Entity* marcher = &state->marchers[i];
            if(marcher->honk_this_cycle) {
                if(i < state->marchers_len - 1) {
                    state->marchers[i + 1].honk_this_cycle = true;
                }
                marcher->honk_this_cycle = false;
            }
        }

        if(state->input_honk) {
            hannah->honk_this_cycle = true;
            state->input_honk = false;
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
                    if(state->signs[i].doublewide && iv2_eq(move_pos, iv2_add(state->signs[i].pos, iv2_new(1, 0)))) {
                        sign = &state->signs[i];
                        break;
                    }
                }

                if(state->egg_exists && state->egg_states[state->egg_index] == EGG_UNBROKEN && iv2_eq(move_pos, state->egg_pos)) {
                    if(state->egg_index == 0) {
                        start_cutscene(game, CUT_EGG_RUMBLE);
                    } else {
                        state->egg_states[state->egg_index] = EGG_BREAKING;
                        game->mode = MODE_EGG_EXPLODE;
                        state->egg_t = 0.0;
                    }
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
            if(duck->move_this_cycle == MOVE_UP)  duck->sprite_handle = SPRITE_DUCK_UP;
            if(duck->move_this_cycle == MOVE_DOWN) duck->sprite_handle = SPRITE_DUCK_DOWN;

            //if(duck->honk_this_cycle) {
            //    if(duck->sprite_handle == SPRITE_DUCK_LEFT) duck->sprite_handle = SPRITE_DUCK_HONK_LEFT;
            //    if(duck->sprite_handle == SPRITE_DUCK_RIGHT) duck->sprite_handle = SPRITE_DUCK_HONK_RIGHT;
            //    if(duck->sprite_handle == SPRITE_DUCK_UP) duck->sprite_handle = SPRITE_DUCK_HONK_UP;
            //    if(duck->sprite_handle == SPRITE_DUCK_DOWN) duck->sprite_handle = SPRITE_DUCK_HONK_DOWN;
            //}
            update_marcher_with_platforms(state, duck, follower, false);
        }

        // NOW: Frogo update
        if(state->frogo_exists) {
            // Check for being alerted and update target
            Entity* frogo = &state->frogo;
            for(MoveDirection i = 1; i < 5; i++) {
                iv2 pos = frogo->pos_cur;
                for(i32 j = 0; j < 6; j++) {
                    pos = relative_pos_after_direction(pos, i);
                    if(!pos_passable(game, pos)) {
                        break;
                    }
                    if(j < 3) {
                        if(any_marcher_at_pos(state, pos)) {
                            if(!state->frogo_alerted) {
                                state->frogo_alerted_just_now = true;
                            }
                            state->frogo_alerted = true;
                            state->frogo_alert_direction = i;
                            break;
                        }
                    } else {
                        if(any_honk_at_pos(state, pos)) {
                            if(!state->frogo_alerted) {
                                state->frogo_alerted_just_now = true;
                            }
                            state->frogo_alerted = true;
                            state->frogo_alert_direction = i;
                            break;
                        }
                    }
                }
            }
        }
        frogo_move(game);
        // Buttons
        for(i32 i = 0; i < state->buttons_len; i++) {
            Button* button = &state->buttons[i];
            ButtonState state_prev = button->state;
            button->state = BUTTON_OFF;
            if(state->frogo_exists && iv2_eq(state->frogo.pos_cur, button->pos)) {
                button->state = BUTTON_ON;
            }
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

        //for(i32 i = 0; i < state->buttons_len; i++) {
        //    Button* button = &state->buttons[i];
        //    if(state->frogo_exists && iv2_eq(state->frogo.pos_cur, button->pos)) {
        //        button->state = BUTTON_ON;
        //    }
        //}

        // Damage crumblers
        for(i32 i = 0; i < state->platforms_len; i++) {
            Entity* platform = &state->platforms[i];
            if(platform->hits_taken == 0) {
                for(i32 j = 0; j < state->marchers_len; j++) {
                    Entity* marcher = &state->marchers[j];
                    if(platform->crumbler && iv2_eq(marcher->pos_cur, platform->pos_cur)) {
                        platform->hits_taken++;
                    }
                }
            } else {
                platform->hits_taken++;
            }
        }
    }

    update_muffin_minigame(game);
    update_snake_minigame(game);

    // draw
    update_visual_state(game, draw_list, v2_zero(), dt);

    if(input_button_pressed(game->input_buttons[BUTTON_GOD])) {
        game->god_mode = !game->god_mode;
    }
    if(game->god_mode) {
        draw_simple_text(draw_list, string_const("GOD"), v2_new(27, 58), (i32)(game->time * 2.0) % 2);
    }
    if(input_button_pressed(game->input_buttons[BUTTON_LEAVE])) dt_mod *= 2.0;
    if(input_button_pressed(game->input_buttons[BUTTON_BECKON])) dt_mod *= 0.5;
    if(within_epsilon(dt_mod, 1.0, 0.1)) {
        dt_mod = 1.0;
    } else {
        char time_buf[16];
        String time_str = string_init(time_buf, 16);
        string_print_int(&time_str, (i32)dt_mod);
        string_cat(&time_str, string_const("X"));
        draw_simple_text(draw_list, time_str, v2_new(29, 53), 0);
    }

    if(!game->first_move_made) {
        draw_sprite_animated(draw_list, SPRITE_WASD, game->time / 4.0, v2_new(28.0, 26.0), 0);
    }

    // Debug circle
    //debug_circle_pos = state->platforms[0].pos_cur;
    debug_circle_pos = hannah->pos_lead;
    //draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, (game->time * 2.0), v2_scale(v2_from_iv2(debug_circle_pos), 8.0f), 0);
    debug_circle_pos = state->hannah_pos_lead_prev;
    //draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, (game->time * 4.0), v2_scale(v2_from_iv2(debug_circle_pos), 8.0f), 0);
    //draw_sprite_animated(draw_list, SPRITE_DEBUG_CIRCLE, ((game->time + 1.0) * 2.0f), v2_scale(v2_from_iv2(debug_circle_pos_2), 8.0f), 0);

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_EDITOR;
	}

    if(switch_to_level != -1) {
        state->level_index_prev = state->level_index;
        state->level_index = switch_to_level;
        state->level_prev_offset_pos = v2_scale(v2_from_iv2(level_switch_offset), 64.0);
        game->transition_t = 0.0;
        game->mode = MODE_LEVEL_SWITCH;

        for(i32 i = 0; i < state->marchers_len; i++) {
            entity_offset_teleport(&state->marchers[i], iv2_scale(level_switch_offset, 8));
        }

        if(state->boat_riding) {
            for(i32 i = 0; i < 5; i++) {
                entity_offset_teleport(&state->platforms[i], iv2_scale(level_switch_offset, 8));
            }
        }

        for(i32 i = 0; i < SIGNS_MAX; i++) {
            state->signs[i].talk_count = 0;
        }

    	for(i32 i = 0; i < state->platforms_len; i++) {
        	Entity* platform = &state->platforms[i];
            platform->hits_taken = 0;
            // NOW: this was previously on every logic update. shouldn't cause problems to do it here.
            platform->sink_state = PLATFORM_FLOAT;
            platform->crumbler = false;
            platform->snake_index = 0;
    	}

    	for(i32 i = 0; i < state->buttons_len; i++) {
        	Button* button = &state->buttons[i];
        	button->state = BUTTON_OFF;

    	}

    	// NOW: get apple ready
    	SnakeGame* snake_game = &state->snake_game;
    	snake_game->apple_active = true;
    	snake_game->apple_pos = iv2_new(1 + random_i32(5), 1 + random_i32(5));

    	state->frogo_alerted = false;
        state->frogo_alert_direction = MOVE_NONE;
        state->frogo.sprite_handle = SPRITE_FROG_LEAP_RIGHT;
    }

}
