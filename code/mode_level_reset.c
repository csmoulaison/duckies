#define LEVEL_RESET_TIME 1.0f

void mode_level_reset_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    game->level_reset_t += dt / LEVEL_RESET_TIME;
    if(game->level_reset_t > 1.0f) {
        game->level_reset_t = 0.0f;
        game->state = game->saved_state;
        game->mode = MODE_GAME;
    }
}
