#include <SDL2/SDL.h>
#include <SDL_opengles2.h>
#include <GLES3/gl3.h>

#include <emscripten.h>

#define CSM_IMPLEMENTATION
#define CSM_MODULE_WINDOW
#define CSM_MODULE_MATH
#define CSM_MODULE_GL
#define CSM_MODULE_FIEDLER
#define CSM_MODULE_DYNAMIC_LIB
#define CSM_MODULE_MESH
#define CSM_MODULE_TEXTURE
#define CSM_MODULE_SPRITE

#define DEBUG_STACK 0
#define DEBUG_CAPACITY_WARNING 0
#define BUFFER_TRACKING 0
#include "csm_core/core.h"

#include "world.c"
#include "generated/asset_handles.c"
#include "generated/asset_data.c"
#include "audio.h"
#include "game.c"
#include "gl_render.c"

#define FRAME_LENGTH 0.01f
#define FRAME_MEMORY_SIZE (MEGABYTE * 1)
#define GAME_MEMORY_SIZE (MEGABYTE * 1)
#define EVENT_BUFFER_SIZE (MEGABYTE)

#define ROOT_MEMORY_SIZE (sizeof(Context) + GAME_MEMORY_SIZE + FRAME_MEMORY_SIZE)

typedef struct {
	SDL_Window*   window;
	SDL_GLContext sdl_gl;
	RendererGL    renderer;
	Audio         audio;
	Stack         game_stack;
	Stack         frame_stack;
	bool          close_requested;
	u64           time_prev;
	u64           time_cur;
	bool          gesture_captured;
	iv2           window_size;
	bool          kill;
} Context;

static void
_set_SDL_Attribute(SDL_GLattr attr, i32 value, const char *attrName)
#define set_SDL_Attribute(x, v) _set_SDL_Attribute(x, v, #x)
{
    if (SDL_GL_SetAttribute(attr, value) != 0) {
        printf("SDL set attrib failed: %s, %s\n", attrName, SDL_GetError());
        panic();
    }
}

void init_after_gesture(Context* ctx) {
    if(ctx->gesture_captured) {
        return;
    }
    ctx->gesture_captured = true;

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec_desired;
    SDL_AudioSpec spec_actual;
    SDL_zero(spec_desired);
    spec_desired.freq     = AUDIO_SAMPLE_RATE;
    spec_desired.format   = AUDIO_F32SYS;
    spec_desired.channels = 1;
    spec_desired.samples  = 1024;
    spec_desired.callback = audio_callback;
    spec_desired.userdata = &ctx->audio;
    if(SDL_OpenAudio(&spec_desired, &spec_actual) < 0) {
        printf("Failed to open audio\n");
        panic();
    }
    SDL_PauseAudio(0);
    audio_init(&ctx->audio);
}

WindowEvent sdl_pull_event(Context* ctx) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            ctx->close_requested = true;
        } else if(event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:      return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_W };
                case SDL_SCANCODE_A:      return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_A };
                case SDL_SCANCODE_S:      return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_S };
                case SDL_SCANCODE_D:      return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_D };
                case SDL_SCANCODE_R:      return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_R };
                case SDL_SCANCODE_ESCAPE: return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_ESCAPE };
                case SDL_SCANCODE_TAB:    return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_TAB };
                case SDL_SCANCODE_SPACE:  return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = KEYCODE_SPACE };
                default: break;
            }
        } else if(event.type == SDL_KEYUP) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:      return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_W };
                case SDL_SCANCODE_A:      return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_A };
                case SDL_SCANCODE_S:      return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_S };
                case SDL_SCANCODE_D:      return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_D };
                case SDL_SCANCODE_R:      return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_R };
                case SDL_SCANCODE_ESCAPE: return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_ESCAPE };
                case SDL_SCANCODE_TAB:    return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_TAB };
                case SDL_SCANCODE_SPACE:  return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = KEYCODE_SPACE };
                default: break;
            }
        } else if(event.type == SDL_WINDOWEVENT) {
            if(event.window.type == SDL_WINDOWEVENT_FOCUS_LOST || event.window.type == SDL_WINDOWEVENT_MINIMIZED) {
                printf("defocus event!\n");
                return (WindowEvent){ .type = WINDOW_EVENT_DEFOCUS };
            } else if(event.window.type == SDL_WINDOWEVENT_RESIZED || event.window.type == SDL_WINDOWEVENT_SIZE_CHANGED) {
                ctx->window_size.x = event.window.data1;
                ctx->window_size.y = event.window.data2;
                ctx->kill = true;

            }
        } else if(event.type == SDL_MOUSEBUTTONDOWN) {
            init_after_gesture(ctx);
        }
    }
    return (WindowEvent){ .type = WINDOW_EVENT_NONE };
}

i32 sdl_pull_all_events(Context* ctx, Buffer* buffer) {
	WindowEvent event;
	i32 count = 0;
	while((event = sdl_pull_event(ctx)).type != WINDOW_EVENT_NONE) {
    	WindowEvent* ptr = (WindowEvent*)&buffer->memory[count * sizeof(WindowEvent)];
    	*ptr = event;
		count++;
	}
    if(count * sizeof(WindowEvent) > buffer->size) {
        printf("sdl_pull_all_events: buffer too small!");
        panic();
    }
	return count;
}

void main_loop(void* mem) {
    Context* ctx = (Context*)mem;
	Buffer event_buffer = stack_from_stack(&ctx->frame_stack, EVENT_BUFFER_SIZE, string_const("Events")).buffer;
	i32 events_len = sdl_pull_all_events(ctx, &event_buffer);
	WindowEvent* events_head = (WindowEvent*)event_buffer.memory;

    if(ctx->window_size.x == 0) {
        iv2 size;
        SDL_GetWindowSize(ctx->window, &size.x, &size.y);
    	draw_init_list(&ctx->renderer.list, size);
    } else {
    	draw_init_list(&ctx->renderer.list, ctx->window_size);
    }

    ctx->time_prev = ctx->time_cur;
    ctx->time_cur = SDL_GetTicks64();
    f64 dt = (double)(ctx->time_cur - ctx->time_prev) / 1000.0;

	if(ctx->kill) {
        dt *= 10.0;
	}

	game_update(ctx->game_stack.memory, &ctx->renderer.list, &ctx->audio, events_head, events_len, dt);
	gl_render_update(&ctx->renderer, asset_pack_data);
	SDL_GL_SwapWindow(ctx->window);
	stack_clear(&ctx->frame_stack);
}

Context* main_init_context() {
    FILE* asset_file = fopen("build/asset/pack.data", "rb");
    fread(_binary_build_asset_pack_data_start, ASSET_PACK_SIZE, 1, asset_file);
    fclose(asset_file);

	void* mem = malloc(ROOT_MEMORY_SIZE);
	Stack root_stack  = stack_from_memory(mem, ROOT_MEMORY_SIZE, string_const("Root"));
	Context* ctx = (Context*)stack_alloc(&root_stack, sizeof(Context));
    ctx->game_stack  = stack_from_stack(&root_stack, GAME_MEMORY_SIZE, string_const("Game"));
    ctx->frame_stack = stack_from_stack(&root_stack, FRAME_MEMORY_SIZE, string_const("Frame"));

    ctx->window = SDL_CreateWindow("Jam", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 640, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    assert(ctx->window != NULL);
    set_SDL_Attribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    set_SDL_Attribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    set_SDL_Attribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    set_SDL_Attribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
    ctx->sdl_gl = SDL_GL_CreateContext(ctx->window);
    assert(ctx->sdl_gl != NULL);

	gl_render_init(&ctx->renderer, asset_pack_data);
	game_init(ctx->game_stack.memory, asset_pack_data);
	emscripten_set_main_loop_arg(main_loop, ctx, 0, 1);
	random_init();
	fast_random_init();

    ctx->time_cur = SDL_GetTicks64();
    ctx->time_prev = 0;

    return ctx;
}

int main() {
    Context* ctx = main_init_context();
    while(true) {
        main_loop(ctx);
    }
	return 0;
}

