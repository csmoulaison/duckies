#define MENU_TO_GAME_TIME 4.0f

void mode_menu_to_game_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    game->transition_t += dt / MENU_TO_GAME_TIME;
    override_pallete_from_fade_t(draw_list, game->transition_t);

    if(game->transition_t > 0.6) {
        update_visual_state(game, draw_list, dt);
        if(game->transition_t > 1.0) {
            game->mode = MODE_GAME;
        }
    } else {
        draw_main_menu(game, draw_list, dt);
    }
}
