#ifndef csm_window_h_INCLUDED
#define csm_window_h_INCLUDED

#ifdef CSM_WINDOW_XLIB
#include <GL/glx.h>
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
#endif

#ifndef WINDOW_KEYCODE_HASHMAP_SIZE
#define WINDOW_KEYCODE_HASHMAP_SIZE 1024
#endif

#ifndef WINDOW_DEBUG_MODE
#define WINDOW_DEBUG_MODE true
#endif

typedef enum {
	KEYCODE_NONE,
	KEYCODE_ESCAPE,
	KEYCODE_SPACE,
	KEYCODE_ENTER,
	KEYCODE_TAB,
	KEYCODE_W,
	KEYCODE_A,
	KEYCODE_S,
	KEYCODE_D,
	KEYCODE_Q,
	KEYCODE_E,
	KEYCODE_R,
	KEYCODE_M,
	KEYCODE_UP,
	KEYCODE_LEFT,
	KEYCODE_DOWN,
	KEYCODE_RIGHT,
} Keycode;

typedef struct {
	u32 key;
	u32 value;
} WindowKeycodeMapping;

typedef struct {
    // TODO: messy
#ifdef CSM_WINDOW_XLIB
	Display* display;
	Window window;
#endif
	iv2 size;
	WindowKeycodeMapping keycode_map[WINDOW_KEYCODE_HASHMAP_SIZE];
	bool viewport_updated;
} WindowContext;

typedef enum {
	WINDOW_EVENT_NONE,
	WINDOW_EVENT_KEYDOWN,
	WINDOW_EVENT_KEYUP,
	WINDOW_EVENT_DEFOCUS
} WindowEventType;

typedef struct {
	WindowEventType type;
	union {
		Keycode keycode;
		iv2 viewport_size;
	};
} WindowEvent;

void window_init_context(WindowContext* ctx, char* window_name);
WindowEvent window_pull_event(WindowContext* window);
i32 window_pull_all_events(WindowContext* ctx, Buffer* buffer);
void window_swap_buffers(WindowContext* ctx);
void window_size(WindowContext* window, i32* width, i32* height);
void window_register_keycode(WindowContext* ctx, u32 keycode);

#ifdef CSM_IMPLEMENTATION
#ifdef CSM_WINDOW_XLIB

u32 _window_keycode_hash(u32 key) {
	u32 h = key * UINT32_C(2654435761);
	return h >> 22;
}

u32 _window_keycode_map_lookup(WindowKeycodeMapping* keycode_map, u32 xlib_keysym) {
	u32 index = _window_keycode_hash(xlib_keysym);
	while(keycode_map[index].key != xlib_keysym) {
		if(keycode_map[index].value == KEYCODE_NONE) {
			return KEYCODE_NONE;
		}
		index = (index + 1) % WINDOW_KEYCODE_HASHMAP_SIZE;
	}
	return keycode_map[index].value;
}

void window_register_keycode(WindowContext* ctx, u32 keycode) {
    WindowKeycodeMapping* keycode_map = ctx->keycode_map;
    u32 xlib_keysym = 0;
    switch(keycode) {
        case KEYCODE_W: {
            xlib_keysym = XK_w;
        } break;
    	case KEYCODE_A: {
        	xlib_keysym = XK_a;
    	} break;
    	case KEYCODE_S: {
        	xlib_keysym = XK_s;
    	} break;
    	case KEYCODE_D: {
        	xlib_keysym = XK_d;
    	} break;
    	case KEYCODE_Q: {
        	xlib_keysym = XK_q;
    	} break;
    	case KEYCODE_E: {
        	xlib_keysym = XK_e;
    	} break;
    	case KEYCODE_R: {
        	xlib_keysym = XK_r;
    	} break;
    	case KEYCODE_M: {
        	xlib_keysym = XK_m;
    	} break;
    	case KEYCODE_UP: {
        	xlib_keysym = XK_Up;
    	} break;
    	case KEYCODE_LEFT: {
        	xlib_keysym = XK_Left;
    	} break;
    	case KEYCODE_DOWN: {
        	xlib_keysym = XK_Down;
    	} break;
    	case KEYCODE_RIGHT: {
        	xlib_keysym = XK_Right;
    	} break;
    	case KEYCODE_ESCAPE: {
        	xlib_keysym = XK_Escape;
    	} break;
    	case KEYCODE_TAB: {
        	xlib_keysym = XK_Tab;
    	} break;
    	case KEYCODE_SPACE: {
        	xlib_keysym = XK_space;
    	} break;
    	case KEYCODE_ENTER: {
        	xlib_keysym = XK_Return;
    	} break;
    	default: break;
    }
    
	u32 index = _window_keycode_hash(xlib_keysym);
	assert(index < WINDOW_KEYCODE_HASHMAP_SIZE);
	while(keycode_map[index].value != KEYCODE_NONE && keycode_map[index].key != xlib_keysym) {
		printf("csm_window: hashmap add collision! %u\n", index);
		index = (index + 1) % WINDOW_KEYCODE_HASHMAP_SIZE;
	}
	keycode_map[index].key = xlib_keysym;
	keycode_map[index].value = keycode;
}

void window_init_context(WindowContext* ctx, char* window_name) {
	ctx->display = XOpenDisplay("");
	assert(ctx->display != NULL);

	// Will factor out GLX stuff for other API integrations.
	i32 glx_version_major;
	i32 glx_version_minor;
	assert(glXQueryVersion(ctx->display, &glx_version_major, &glx_version_minor) != 0 && !(glx_version_major == 1 && glx_version_minor < 3) && glx_version_major >= 1);

	// Find the best framebuffer configuration from those available. Our only
	// quantitative criteria at the moment is sample count.
	i32 desired_framebuffer_attributes[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		GLX_SAMPLE_BUFFERS, 1,
		GLX_SAMPLES, 4,
		None
	};
	i32 framebuffer_configs_len;
	GLXFBConfig* framebuffer_configs = glXChooseFBConfig(ctx->display, DefaultScreen(ctx->display), desired_framebuffer_attributes, &framebuffer_configs_len);
	assert(framebuffer_configs != NULL);
	i32 best_framebuffer_config = -1;
	i32 best_sample_count = -1;
	for(i32 fc = 0; fc < framebuffer_configs_len; fc++) {
		XVisualInfo* tmp_visual_info = glXGetVisualFromFBConfig(ctx->display, framebuffer_configs[fc]);
		if(tmp_visual_info != NULL) {
			i32 sample_buffers;
			glXGetFBConfigAttrib(ctx->display, framebuffer_configs[fc], GLX_SAMPLE_BUFFERS, &sample_buffers);
			i32 samples;
			glXGetFBConfigAttrib(ctx->display, framebuffer_configs[fc], GLX_SAMPLES, &samples);
			if(best_framebuffer_config == -1 || (sample_buffers && samples > best_sample_count)) {
				best_framebuffer_config = fc;
				best_sample_count = samples;
			}
		}
		XFree(tmp_visual_info);
	}
	GLXFBConfig glx_framebuffer_config = framebuffer_configs[best_framebuffer_config];
	XVisualInfo* glx_visual_info = glXGetVisualFromFBConfig(ctx->display, glx_framebuffer_config);
	XFree(framebuffer_configs);

	// Set up root ctx-> This is somewhat mixed up with GLX stuff still, though
	// it should survive the generic case with some things factored into variables.
	Window root_window = RootWindow(ctx->display, glx_visual_info->screen);
	XSetWindowAttributes set_window_attributes = {};
	set_window_attributes.colormap = XCreateColormap(ctx->display, root_window, glx_visual_info->visual, AllocNone);
	set_window_attributes.background_pixmap = None;
	set_window_attributes.border_pixel = 0;
	set_window_attributes.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;

	// Create our actual Xlib ctx->
	u32 window_width = 1;
	u32 window_height = 1;
	ctx->window = XCreateWindow(ctx->display, root_window, 0, 0, window_width, window_height, 0, glx_visual_info->depth, InputOutput, glx_visual_info->visual, CWBorderPixel | CWColormap | CWEventMask, &set_window_attributes);
	if(ctx->window == 0) { panic(); }
	XFree(glx_visual_info);
	XStoreName(ctx->display, ctx->window, window_name);
	XMapWindow(ctx->display, ctx->window);

	// Validate existence of required GL extensions
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
	char* gl_extensions = (char*)glXQueryExtensionsString(ctx->display, DefaultScreen(ctx->display));
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	const char* extension = "GLX_ARB_create_context";
	char* start;
	char* where;
	char* terminator;
	// Extension names shouldn't have spaces
	where = strchr((char*)extension, ' ');
	assert(!where && *extension != '\0');
	bool found_extension = true;
	for (start = gl_extensions;;) {
		where = strstr(start, extension);
		if (!where)
			break;

		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ') {
			if (*terminator == ' ' || *terminator == '\0')
				found_extension = true;
			start = terminator;
		}
	}
	assert(found_extension == true);

	// Create GLX context and window
	i32 glx_attributes[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 6,
		None
	};
	GLXContext glx = glXCreateContextAttribsARB(ctx->display, glx_framebuffer_config, 0, 1, glx_attributes);
	if(glXIsDirect(ctx->display, glx) == false) { panic(); }
	glXMakeCurrent(ctx->display, ctx->window, glx);
}

WindowEvent window_pull_event(WindowContext* ctx) {
retry:
	if(!XPending(ctx->display)) {
		return (WindowEvent){ .type = WINDOW_EVENT_NONE };
	}

	XEvent event;
	XNextEvent(ctx->display, &event);
	switch(event.type) {
		case Expose:
			break;
		case ConfigureNotify: {
			window_size(ctx, &ctx->size.x, &ctx->size.y);
			ctx->viewport_updated = true;
		} break;
		case KeyPress: {
			u32 keysym = XLookupKeysym(&(event.xkey), 0);
			u32 keycode = _window_keycode_map_lookup(ctx->keycode_map, keysym);
			return (WindowEvent){ .type = WINDOW_EVENT_KEYDOWN, .keycode = keycode };
		} break;
		case KeyRelease: {
			// X11 natively repeats key events when the key is held down. We could turn
			// that off, but it turns it off globally for the user's X11 session, which
			// is unacceptable of course. Here we are ignoring them manually.
			bool is_repeat_key = false;
            if (XPending(ctx->display)) {
				XEvent next_event;
                XPeekEvent(ctx->display, &next_event);
                if (next_event.type == KeyPress && next_event.xkey.time == event.xkey.time 
                && next_event.xkey.keycode == event.xkey.keycode) {
					XNextEvent(ctx->display, &next_event);
					is_repeat_key = true;
                }
            }
			if(!is_repeat_key) {
				u64 keysym = XLookupKeysym(&(event.xkey), 0);
				u32 keycode = _window_keycode_map_lookup(ctx->keycode_map, keysym);
				return (WindowEvent){ .type = WINDOW_EVENT_KEYUP, .keycode = keycode };
			}
		} break;
		default: break;
	}
	goto retry;
}

// Places events in the stack and returns the number of events pulled
// TODO: grab SDL implementation from jam
i32 window_pull_all_events(WindowContext* ctx, Buffer* buffer) {
	WindowEvent event;
	i32 count = 0;
	while((event = window_pull_event(ctx)).type != WINDOW_EVENT_NONE) {
    	WindowEvent* ptr = (WindowEvent*)&buffer->memory[count * sizeof(WindowEvent)];
    	*ptr = event;
		count++;
	}
#if WINDOW_DEBUG_MODE
    if(count * sizeof(WindowEvent) > buffer->size) {
        log_fail("window_pull_all_events: buffer too small!");
    }
#endif
	return count;
}

void window_swap_buffers(WindowContext* ctx) {
	glXSwapBuffers(ctx->display, ctx->window);
}

void window_size(WindowContext* ctx, i32* width, i32* height) {
	XWindowAttributes window_attributes;
	XGetWindowAttributes(ctx->display, ctx->window, &window_attributes);
	*width = window_attributes.width;
	*height = window_attributes.height;
}

#endif
#endif // CSM_IMPLEMENTATION
#endif // window_h_INCLUDED
