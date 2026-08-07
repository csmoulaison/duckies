#define LEVEL_RESET_TIME 1.0f

void mode_level_reset_update(Game* game, DrawList* draw_list, Audio* audio, f32 dt) {
    AudioWaveChannel* wave = &audio->wave_channels[3];

    game->level_reset_t += dt / LEVEL_RESET_TIME;
    if(game->level_reset_t > 1.0) {
        game->level_reset_t = 0.0;
        game->state = game->saved_state;
        game->mode = MODE_GAME;

        wave->amp = 0.0;
        wave->freq = 0.0;
    } else {
        wave->amp = 0.5 - game->level_reset_t;
        wave->freq = 300 - sin(game->level_reset_t * 60.0) * 300.0 - game->level_reset_t * 300.0;
    }
    update_visual_state(game, draw_list, dt);

}
