void push_world_lstack(i16* lstack, iv2* lstack_positions, iv2* active_positions, i32* lstack_len, i32 level, iv2 pos) {
    active_positions[level] = pos;
    lstack[*lstack_len] = level;
    lstack_positions[*lstack_len] = pos;
    *lstack_len += 1;
}

void mode_world_viewer_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt, Stack* stack) {
	draw_list->minified = true;

	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	File save_file = file_open(string_const(WORLD_PATH_FROM_BIN), FILE_OPEN_WRITE);
    	world_save(&save_file, game->world);
    	draw_list->minified = false;
    	game->mode = MODE_GAME;
	}

    Level* levels    = game->world->levels;
    i32 index_cur    = game->state.level_index;
    Level* level_cur = active_game_level(game);

    u8* active = (u8*)stack_alloc_zero(stack, sizeof(u8) * LEVELS_MAX);
    iv2* active_positions = (iv2*)stack_alloc_zero(stack, sizeof(iv2) * LEVELS_MAX);
    active[game->state.level_index] = 1;
    active_positions[game->state.level_index] = iv2_new(0, 0);

    i16* lstack = (i16*)stack_alloc_zero(stack, sizeof(i16) * LEVELS_MAX);
    iv2* lstack_positions = (iv2*)stack_alloc_zero(stack, sizeof(iv2) * LEVELS_MAX);
    i32 lstack_len = 1;
    lstack[0] = index_cur;
    lstack_positions[0] = iv2_new(0, 0);

    i32 count = 0;
    while(lstack_len > 0) {
        i32    index = lstack[lstack_len - 1];
        iv2    ipos  = lstack_positions[lstack_len - 1];
        v2     pos   = v2_scale(v2_from_iv2(ipos), 64.0);
        lstack_len--;
        active[index] = 1;

        Level* level = &levels[index];
        if(pos.y > -96.0) {
            printf("drawing %d at pos %f %f\n", count, pos.x, pos.y);
            draw_level_tiles(level, draw_list, pos);
        }

        if(level->exit_up != 0 && active[level->exit_up] == 0) {
            push_world_lstack(lstack, lstack_positions, active_positions, &lstack_len, level->exit_up,    iv2_add(ipos, iv2_new(0, 1)));
        }
        if(level->exit_left != 0 && active[level->exit_left] == 0) {
            push_world_lstack(lstack, lstack_positions, active_positions, &lstack_len, level->exit_left,  iv2_add(ipos, iv2_new(-1, 0)));
        }
        if(level->exit_down != 0 && active[level->exit_down] == 0) {
            push_world_lstack(lstack, lstack_positions, active_positions, &lstack_len, level->exit_down,  iv2_add(ipos, iv2_new(0, -1)));
        }
        if(level->exit_right != 0 && active[level->exit_right] == 0) {
            push_world_lstack(lstack, lstack_positions, active_positions, &lstack_len, level->exit_right, iv2_add(ipos, iv2_new(1, 0)));
        }
        printf("lstack = %d\n", lstack_len);
    }

    switch(game->world_viewer_mode) {
        // Lookaround mode
        case 0: {
            if(input_button_pressed(game->input_buttons[BUTTON_UP])) {
            	if(level_cur->exit_up != 0) {
                	game->state.level_index = level_cur->exit_up;
            	} else {
                    game->world_viewer_mode = 1;
                    game->world_viewer_place_offset = iv2_new(0, 1);
            	}
            } else if(input_button_pressed(game->input_buttons[BUTTON_LEFT])) {
            	if(level_cur->exit_left != 0) {
                	game->state.level_index = level_cur->exit_left;
            	} else {
                    game->world_viewer_mode = 1;
                    game->world_viewer_place_offset = iv2_new(-1, 0);
            	}
        	} else if(input_button_pressed(game->input_buttons[BUTTON_DOWN])) {
            	if(level_cur->exit_down != 0) {
                	game->state.level_index = level_cur->exit_down;
            	} else {
                    game->world_viewer_mode = 1;
                    game->world_viewer_place_offset = iv2_new(0, -1);
            	}
        	} else if(input_button_pressed(game->input_buttons[BUTTON_RIGHT])) {
            	if(level_cur->exit_right != 0) {
                	game->state.level_index = level_cur->exit_right;
            	} else {
                    game->world_viewer_mode = 1;
                    game->world_viewer_place_offset = iv2_new(1, 0);
            	}
        	}

            if(input_button_pressed(game->input_buttons[BUTTON_QUIT])) {

            }

            draw_list->minified = false;
            draw_num(draw_list, game->state.level_index, v2_new(0.0, 0.0));
            draw_list->minified = true;
        } break;
        // Select level mode
        case 1: {
            // Pack all unplaced levels into an array.
            i16* banked = (i16*)stack_alloc(stack, sizeof(i16) * LEVELS_MAX);
            i32 banked_len = 0;
            for(i32 i = 1; i < LEVELS_MAX; i++) {
                if(active[i] == 0) {
                    banked[banked_len] = i;
                    banked_len++;
                }
            }

            // Control bank selection
            if(input_button_pressed(game->input_buttons[BUTTON_LEFT])) {
                game->world_viewer_level_select_index--;
            }
            if(input_button_pressed(game->input_buttons[BUTTON_RIGHT])) {
                game->world_viewer_level_select_index++;
            }
            if(game->world_viewer_level_select_index <= 0) {
                game->world_viewer_level_select_index = 0;
            } else if(game->world_viewer_level_select_index >= LEVELS_MAX) {
                game->world_viewer_level_select_index = LEVELS_MAX - 1;
            }

            // Draw selection in place
            Level* place_level = &levels[banked[game->world_viewer_level_select_index]];
            v2 draw_pos = v2_scale(v2_from_iv2(game->world_viewer_place_offset), 64.0);
            draw_level_tiles(place_level, draw_list, draw_pos);

            // Draw list below
            for(i32 i = -5; i < 4; i++) {
                i32 banked_index = game->world_viewer_level_select_index + i;
                if(banked_index < 0) continue;
                if(banked_index >= banked_len) continue;

                Level* draw_level = &levels[banked[banked_index]];
                f32 x = i * 96.0;
                draw_level_tiles(draw_level, draw_list, v2_new(x, -224.0));
            }

            // Draw selection level index
            draw_list->minified = false;
            draw_num(draw_list, banked[game->world_viewer_level_select_index], v2_new(0.0, 0.0));
            draw_list->minified = true;

            // Place selection, check every direction for active levels and
            // fixing their exit indices.
            if(input_button_pressed(game->input_buttons[BUTTON_EDITOR_PLACE])) {
                iv2 cardinals[4] = { iv2_new(0, 1), iv2_new(-1, 0), iv2_new(0, -1), iv2_new(1, 0) };
                // Offsets into level struct, mapping up to down, left to right, etc and so on
                i32 other_exit_offsets[4] = { 2, 3, 0, 1 };
                for(i32 i = 0; i < LEVELS_MAX; i++) {
                    if(active[i] == 0) {
                        continue;
                    }
                    for(i32 j = 0; j < 4; j++) {
                        iv2 active_pos = active_positions[i];
                        if(iv2_eq(active_pos, iv2_add(game->world_viewer_place_offset, cardinals[j]))) {
                            printf("its me!\n");
                            // VOLATILE: Dangerous casts taking advantage of level format.
                            i16* other = (i16*)(&levels[i]);
                            i16* place = (i16*)(place_level);
                            other[other_exit_offsets[j]] = banked[game->world_viewer_level_select_index];
                            place[j] = i;
                        }
                    }
                }
                game->world_viewer_mode = 0;
            } else if(input_button_pressed(game->input_buttons[BUTTON_QUIT])) {
                game->world_viewer_mode = 0;
            }
        } break;
    }

    draw_list->minified = false;
}
