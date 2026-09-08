#define LEVEL_RESET_TIME 2.0f

void mode_level_reset_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    AudioWaveChannel* wave = &audio->wave_channels[3];

    f32 old_reset_t = game->level_reset_t;
    game->level_reset_t += dt / LEVEL_RESET_TIME;
    if(old_reset_t < 0.6 && game->level_reset_t > 0.6) {
        game->state = game->saved_state;
    }
    if(game->level_reset_t > 1.0) {
        game->level_reset_t = 0.0;
        game->mode = MODE_GAME;

        wave->amp = 0.0;
        wave->freq = 0.0;
    } else {
        wave->amp = 0.5 - game->level_reset_t;
        wave->freq = 300 - sin(game->level_reset_t * 60.0) * 300.0 - game->level_reset_t * 300.0;
    }
    game->state.input_move = MOVE_NONE;

    override_pallete_from_fade_t(draw_list, game->level_reset_t);
    update_visual_state(game, draw_list, v2_zero(), dt);
}
