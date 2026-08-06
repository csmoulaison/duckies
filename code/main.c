#define CSM_IMPLEMENTATION
#define CSM_MODULE_MATH
#define CSM_MODULE_WINDOW
#define CSM_WINDOW_XLIB
#define CSM_MODULE_GL
#define CSM_MODULE_DYNAMIC_LIB
#define CSM_MODULE_MESH
#define CSM_MODULE_TEXTURE
#define CSM_MODULE_SPRITE
#define CSM_MODULE_ALSA

#define DEBUG_STACK 0
#define BUFFER_TRACKING 1
#include "csm_core/core.h"

#include "world.c"

#include "generated/asset_handles.c"
#include "generated/asset_data.c"
#include "game.h"
#include "gl_render.c"
#include "audio.c"

#define FRAME_LENGTH 0.01f
#define FRAME_MEMORY_SIZE (MEGABYTE * 16)
#define GAME_MEMORY_SIZE (MEGABYTE * 16)
#define EVENT_BUFFER_SIZE (MEGABYTE)

#define ROOT_MEMORY_SIZE (sizeof(Context) + GAME_MEMORY_SIZE + FRAME_MEMORY_SIZE)

typedef struct {
	void*               handle;
	f64                 last_modified;
	GameInitFunction*   init;
	GameUpdateFunction* update;
} GameLibrary;

typedef struct {
	WindowContext window;
	RendererGL    renderer;
	Audio         audio;
	AlsaDevice    alsa;
} Context;

void update_game_library(GameLibrary* game) {
	if(dynamic_lib_update_library(&game->handle, "jam", &game->last_modified)) {
		game->init   = dynamic_lib_load_function(game->handle, "game_init");
		game->update = dynamic_lib_load_function(game->handle, "game_update");
	}
}

i32 main(i32 argc, char** argv) {
	// memory, window, and renderer
	void* mem = malloc(ROOT_MEMORY_SIZE);
	printf("sizeof context %lu\n", sizeof(Context));
	Stack root_stack  = stack_from_memory(mem, ROOT_MEMORY_SIZE, string_const("Root"));
	Context* ctx = (Context*)stack_alloc(&root_stack, sizeof(Context));
    Stack game_stack  = stack_from_stack(&root_stack, GAME_MEMORY_SIZE, string_const("Game"));
    Stack frame_stack = stack_from_stack(&root_stack, FRAME_MEMORY_SIZE, string_const("Frame"));

	window_init_context(&ctx->window, "Jam");
	gl_render_init(&ctx->renderer, asset_pack_data);

	window_register_keycode(&ctx->window, KEYCODE_W);
	window_register_keycode(&ctx->window, KEYCODE_A);
	window_register_keycode(&ctx->window, KEYCODE_S);
	window_register_keycode(&ctx->window, KEYCODE_D);
	window_register_keycode(&ctx->window, KEYCODE_Q);
	window_register_keycode(&ctx->window, KEYCODE_E);
	window_register_keycode(&ctx->window, KEYCODE_R);
	window_register_keycode(&ctx->window, KEYCODE_UP);
	window_register_keycode(&ctx->window, KEYCODE_LEFT);
	window_register_keycode(&ctx->window, KEYCODE_DOWN);
	window_register_keycode(&ctx->window, KEYCODE_RIGHT);
	window_register_keycode(&ctx->window, KEYCODE_ESCAPE);
	window_register_keycode(&ctx->window, KEYCODE_TAB);
	window_register_keycode(&ctx->window, KEYCODE_SPACE);
	window_register_keycode(&ctx->window, KEYCODE_ENTER);

	// game library
	GameLibrary game = {};
	update_game_library(&game);
	game.init(game_stack.memory, asset_pack_data);
	alsa_init(&ctx->alsa, AUDIO_SAMPLE_RATE);
	audio_init(&ctx->audio);

	// main loop
	while(true) {
		Buffer event_buffer = stack_from_stack(&frame_stack, EVENT_BUFFER_SIZE, string_const("Events")).buffer;
		i32 events_len = window_pull_all_events(&ctx->window, &event_buffer);
		WindowEvent* events_head = (WindowEvent*)event_buffer.memory;
		draw_init_list(&ctx->renderer.list, ctx->window.size);
		// NOW: calc delta time
		game.update(game_stack.memory, &ctx->renderer.list, &ctx->audio, events_head, events_len, 0.025f);
		i32 sound_samples_count = alsa_write_samples_count(&ctx->alsa);
		if(sound_samples_count > 0) {
    		f32 sample_buffer[sound_samples_count] = {};
			audio_callback(&ctx->audio, (u8*)sample_buffer, sound_samples_count * 4);
			alsa_write_samples_normalized(&ctx->alsa, sample_buffer, sound_samples_count);
		}
		stack_clear(&frame_stack);
		gl_render_update(&ctx->renderer, asset_pack_data);
		update_game_library(&game);
		window_swap_buffers(&ctx->window);
	}
	return 0;
}
