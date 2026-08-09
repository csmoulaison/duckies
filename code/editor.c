void mode_editor_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    Editor* editor = &game->editor;
    World*  world  = game->world;
    Level*  level  = active_game_level(game);
    LevelState* state = &game->state;

    // Toggle editor
	if(input_button_pressed(game->input_buttons[BUTTON_EDITOR])) {
    	game->mode = MODE_WORLD_VIEWER;
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
    draw_level_tiles(level, draw_list, v2_zero());
    u64 cursor_sprite = sprite_from_tile(editor->cursor_tile);
    v2 pixel_cursor_pos = v2_from_iv2(iv2_scale(editor->cursor_pos, 8));
    draw_sprite(draw_list, cursor_sprite, 0, pixel_cursor_pos, 0);
    draw_sprite_animated(draw_list, SPRITE_CURSOR, (f32)(game->frames_since_init / 60.0f), v2_sub(pixel_cursor_pos, v2_new(1.0, 1.0)), 0);
    draw_num(draw_list, state->level_index, v2_new(0.0, 0.0));
}
