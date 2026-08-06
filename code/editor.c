void mode_editor_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    Editor* editor = &game->editor;
    World*  world  = game->world;
    Level*  level  = active_game_level(game);

    // Toggle editor
	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	File save_file = file_open(string_const(WORLD_PATH_FROM_BIN), FILE_OPEN_WRITE);
    	world_save(&save_file, game->world);
    	game->mode = MODE_GAME;
	}

    // Select tile type
	if(input_button_pressed(game->input_buttons[BUTTON_LEAVE])) {
    	editor->cursor_tile.type--;
	}
	if(input_button_pressed(game->input_buttons[BUTTON_BECKON])) {
    	editor->cursor_tile.type++;
	}
    if(editor->cursor_tile.type == TILE_TYPE_COUNT) {
        editor->cursor_tile.type = 0;
    } else if(editor->cursor_tile.type > TILE_TYPE_COUNT) {
        editor->cursor_tile.type = TILE_TYPE_COUNT - 1;
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
            level->tiles[index_from_pos(pos)] = editor->cursor_tile;
    	}
	}

    // Draw
    draw_level_tiles(level, draw_list);
    u64 cursor_sprite = sprite_from_tile(editor->cursor_tile);
    v2 pixel_cursor_pos = v2_from_iv2(iv2_scale(editor->cursor_pos, 8));
    draw_sprite(draw_list, cursor_sprite, 0, pixel_cursor_pos, 0);
    draw_sprite_animated(draw_list, SPRITE_CURSOR, (f32)(game->frames_since_init / 60.0f), v2_sub(pixel_cursor_pos, v2_new(1.0, 1.0)), 0);

    i8 num_ones = game->level_index % 10;
    i8 num_tens = game->level_index / 10;
    i32 num_palette = 0;
    f32 num_off = 0.0;
    if(game->level_index > 99) {
        num_palette = 1;
    }
    if(game->level_index > 9) {
        draw_sprite(draw_list, SPRITE_NUMS, num_tens, v2_new(0.0, 0.0), num_palette);
        num_off = 4.0;
    }
    draw_sprite(draw_list, SPRITE_NUMS, num_ones, v2_new(num_off, 0.0), num_palette);
}
