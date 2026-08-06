#ifndef game_h_INCLUDED
#define game_h_INCLUDED

#include "draw_list.h"
#include "audio.h"

#define GAME_INIT(name) void name(void* game_memory, void* asset_memory)
typedef GAME_INIT(GameInitFunction);
GAME_INIT(game_init_stub) {}

#define GAME_UPDATE(name) void name(void* game_memory, DrawList* draw_list, Audio* audio, WindowEvent* events, i32 events_len, f32 dt)
typedef GAME_UPDATE(GameUpdateFunction);
GAME_UPDATE(game_update_stub) {}

#endif // game_h_INCLUDED
