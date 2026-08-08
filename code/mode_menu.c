void draw_main_menu(Game* game, DrawList* draw_list, f32 dt) {
    // Tiles
    for(i32 y = 0; y < 10; y++) {
        for(i32 x = 0; x < 10; x++) {
            f32 t = game->time * 8.0;
            t = fmod(t, 16.0);
            draw_sprite(draw_list, SPRITE_GRASS, (x + y) % 2, v2_new((x - 2) * 8.0 + t, (y - 2) * 8.0), 0);
        }
    }

    // Title
    i32 offx = 9.0;
    i32 offy = 32.0;
    i32 font_sz = 9.0;
    String s[2] = {
        string_const("MARCH"),
        string_const("DUCK")
    };
    for(i32 i = 0; i < 2; i++) {
        for(i32 j = 0; j < s[i].len; j++) {
            f32 ysin = sin((game->time * M_PI) + j) * 1.0;
            for(i32 k = 1; k >= 0; k--) {
                draw_sprite(draw_list, SPRITE_FONT_BIG, (i32)s[i].text[j] - 32, v2_new(k + offx + (j * font_sz), -k + offy + i * font_sz + ysin), k * 2);
            }
        }
    }
    f32 ysin = sin((game->time * M_PI) + 4) * 1.0;
    draw_sprite_animated(draw_list, SPRITE_DUCK_RIGHT, game->time * 0.5, v2_new(offx + (4 * font_sz), offy + font_sz + ysin), 0);

    // Hannah
    draw_sprite_animated(draw_list, SPRITE_HANNAH_LEFT, game->time * 0.5, v2_new(-fmod((game->time * 8.0), 96.0f) + 70.0f, 0.0), 0);
}

void mode_menu_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    // Control
	if(input_button_pressed(game->input_buttons[BUTTON_DOWN])) {
        game->transition_t = 0.0;
        game->mode = MODE_MENU_TO_GAME;
	}
    draw_main_menu(game, draw_list, dt);
}
