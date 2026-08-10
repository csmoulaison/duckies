#define TILE_SUB_COUNT 4

void mode_editor_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    Editor* editor = &game->editor;
    World*  world  = game->world;
    Level*  level  = active_game_level(game);
    LevelState* state = &game->state;

    // Toggle editor
	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	if(editor->type == EDIT_TYPE) {
        	editor->type = EDIT_FLAGS;
        } else if(editor->type == EDIT_FLAGS) {
            editor->type = EDIT_SUBTYPE;
            editor->cursor_tile.flags = 0;
    	} else if(editor->type == EDIT_SUBTYPE) {
        	editor->type = EDIT_TYPE;
        	game->mode = MODE_WORLD_VIEWER;
    	}
	}

    // Select tile type
    switch(editor->type) {
        case EDIT_TYPE: {
        	if(input_button_pressed(game->input_buttons[BUTTON_BECKON])) {
            	editor->cursor_tile.type--;
        	}
        	if(input_button_pressed(game->input_buttons[BUTTON_LEAVE])) {
            	editor->cursor_tile.type++;
        	}
            if(editor->cursor_tile.type == TILE_TYPE_COUNT) {
                editor->cursor_tile.type = 0;
            } else if(editor->cursor_tile.type > TILE_TYPE_COUNT) {
                editor->cursor_tile.type = TILE_TYPE_COUNT - 1;
            }
        } break;
        case EDIT_FLAGS: {
        	if(input_button_pressed(game->input_buttons[BUTTON_BECKON])) {
            	if(editor->cursor_flag_index < 1) {
                	editor->cursor_flag_index = 7;
            	} else {
                	editor->cursor_flag_index--;
            	}
        	}
        	if(input_button_pressed(game->input_buttons[BUTTON_LEAVE])) {
            	if(editor->cursor_flag_index >= 7) {
                	editor->cursor_flag_index = 0;
            	} else {
                	editor->cursor_flag_index++;
            	}
        	}
            editor->cursor_tile.flags = 1 << editor->cursor_flag_index;
        } break;
        case EDIT_SUBTYPE: {
        	if(input_button_pressed(game->input_buttons[BUTTON_BECKON])) {
            	editor->cursor_tile.subtype--;
        	}
        	if(input_button_pressed(game->input_buttons[BUTTON_LEAVE])) {
            	editor->cursor_tile.subtype++;
        	}
            if(editor->cursor_tile.subtype == TILE_SUB_COUNT) {
                editor->cursor_tile.subtype = 0;
            } else if(editor->cursor_tile.subtype > TILE_SUB_COUNT) {
                editor->cursor_tile.subtype = TILE_SUB_COUNT - 1;
            }
        } break;
        default: printf("editor type not implemented!\n"); panic();
    }

    if(input_button_pressed(game->input_buttons[BUTTON_QUIT])) {
    	File save_file = file_open(string_const(WORLD_PATH_FROM_BIN), FILE_OPEN_WRITE);
    	world_save(&save_file, game->world);
    	game->mode = MODE_GAME;
    }

    // Move cursor
    iv2 cursor_delta = {};
	if(input_button_pressed(game->input_buttons[BUTTON_UP]))    cursor_delta.y++;
	if(input_button_pressed(game->input_buttons[BUTTON_LEFT]))  cursor_delta.x--;
	if(input_button_pressed(game->input_buttons[BUTTON_DOWN]))  cursor_delta.y--;
	if(input_button_pressed(game->input_buttons[BUTTON_RIGHT])) cursor_delta.x++;
	editor->cursor_pos = iv2_add(editor->cursor_pos, cursor_delta);

	// Place cursor
	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR_PLACE])) {
    	iv2 pos = editor->cursor_pos;
    	if(pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8) {
        	i32 idx = index_from_pos(pos);
        	switch(editor->type) {
            	case EDIT_TYPE: level->tiles[idx].type = editor->cursor_tile.type; break;
            	case EDIT_FLAGS: level->tiles[idx].flags ^= editor->cursor_tile.flags; break;
            	case EDIT_SUBTYPE: level->tiles[idx].subtype = editor->cursor_tile.subtype; break;
            	default: panic();
        	}
    	}
	}

    // Draw
    draw_level_tiles(game, level, draw_list, v2_zero());
    u64 cursor_type_sprite = type_sprite_from_tile(editor->cursor_tile);
    u64 cursor_flag_sprite = flag_sprite_from_tile(editor->cursor_tile);
    v2 pixel_cursor_pos = v2_from_iv2(iv2_scale(editor->cursor_pos, 8));
    draw_sprite(draw_list, cursor_type_sprite, 0, pixel_cursor_pos, 0);
    draw_sprite(draw_list, cursor_flag_sprite, 0, pixel_cursor_pos, 0);
    draw_sprite_animated(draw_list, SPRITE_CURSOR, (f32)(game->frames_since_init / 60.0f), v2_sub(pixel_cursor_pos, v2_new(1.0, 1.0)), 0);
    draw_num(draw_list, state->level_index, v2_new(0.0, 0.0));

    char buf[16];
    String s = string_init(buf, 16);
    switch(editor->type) {
        case EDIT_TYPE: {
            string_cat(&s, string_const("TYPE: "));
            string_print_int(&s, editor->cursor_tile.type);
            draw_simple_text(draw_list, s, v2_new(0, 60), 1);
        } break;
        case EDIT_FLAGS: {
            string_cat(&s, string_const("FLAGS: "));
            string_print_int(&s, editor->cursor_flag_index);
            draw_simple_text(draw_list, s, v2_new(0, 60), 1);
        } break;
        case EDIT_SUBTYPE: {
            string_cat(&s, string_const("SUBTYPE: "));
            string_print_int(&s, editor->cursor_tile.subtype);
            draw_simple_text(draw_list, s, v2_new(0, 60), 1);
        } break;
        default: printf("type %d\n", editor->type); panic();
    }
}
