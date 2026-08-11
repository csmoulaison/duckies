#ifndef __EMSCRIPTEN__

#define CSM_IMPLEMENTATION
#define CSM_MODULE_MATH
#define CSM_MODULE_WINDOW
#define CSM_MODULE_MESH
#define CSM_MODULE_TEXTURE
#define CSM_MODULE_SPRITE
#define BUFFER_TRACKING 1
#include "csm_core/core.h"

#include "world.c"
#include "generated/asset_handles.c"
#include "game.h"
#include "input.c"

char* asset_pack_data = NULL;
#include "draw.c"

#else

#include "game.h"
#include "audio.h"
#include "input.c"
#include "draw.c"

#endif

#define DUCKS_MAX          16
#define MARCHERS_MAX       1 + DUCKS_MAX
#define PLATFORMS_MAX      32
#define CARS_MAX           32
#define BUTTONS_MAX        16
#define GATES_MAX          16
#define GATE_BUTTONS_MAX   16
#define PERMAGATES_MAX     64
#define SIGNS_MAX          8

#define TIME_SCALE         1.25f
#define MOVE_QUEUE_MAX     64
#define LOGIC_DATA_SIZE    256
#define MOVE_CHAIN_SIZE    256
#define MSG_LEN_MAX        16

#define STARTING_DUCKS     2

f32 debug_timescale = 1.0;

typedef enum {
    MOVE_NONE  = 0,
    MOVE_UP    = 1,
    MOVE_LEFT  = 2,
    MOVE_DOWN  = 3,
    MOVE_RIGHT = 4
} MoveDirection;

typedef enum {
    PLATFORM_FLOAT,
    PLATFORM_WARN,
    PLATFORM_SINK
} PlatformSinkState;

typedef struct {
    u64               sprite_handle;
    iv2               pos_cur;
    iv2               pos_prev;
    iv2               pos_lead;
    v2                pos_visible;
    v2                pos_prev_visible;
    f32               pos_t;
    f32               anim_offset_t;
    MoveDirection     move_this_cycle;
    MoveDirection     pulled_move_this_cycle;
    PlatformSinkState sink_state;
    i32               car_width;
    i32               palette_swap;
} Entity;

typedef enum {
    EDIT_TYPE = 0,
    EDIT_FLAGS,
    EDIT_SUBTYPE,
    EDIT_COUNT
} EditorType;

typedef struct {
    EditorType type;
    i32  level_index;
    iv2  cursor_pos;
    Tile cursor_tile;
    u16  cursor_flag_index;
} Editor;

typedef enum {
    MODE_PREMENU = 0,
    MODE_MENU,
    MODE_MENU_TO_GAME,
    MODE_GAME,
    MODE_LEVEL_RESET,
    MODE_LEVEL_SWITCH,
    MODE_MSG,
    MODE_CUTSCENE,
    MODE_EGG_EXPLODE,
    MODE_EGG_COLLECT,
    MODE_EDITOR,
    MODE_WORLD_VIEWER
} GameMode;

typedef enum {
    CUT_NONE,
    CUT_FROG_PAUSE,
    CUT_FROG_INVITATION,
    CUT_FROG_INVITATION_2
} CutsceneMode;

typedef enum {
    EGG_UNBROKEN,
    EGG_BREAKING,
    EGG_BROKEN,
    EGG_COLLECTED
} EggState;

typedef enum {
    BUTTON_OFF,
    BUTTON_ON
} ButtonState;

typedef struct {
    ButtonState state;
    iv2         pos;
    f32         transition_t;
} Button;

typedef enum {
    GATE_CLOSED,
    GATE_OPEN
} GateState;

typedef enum {
    TRIGGER_NONE,
    TRIGGER_BUTTONS
} GateTrigger;

typedef struct {
    GateState   state;
    iv2         pos;
    f32         transition_t;
    i32         permagate_index;

    GateTrigger trigger_type;
    union {
        struct {
            u8 indices[GATE_BUTTONS_MAX];
            u8 len;
        } buttons;
    } trigger;
} Gate;

typedef struct {
    i32 index;
    iv2 pos;
    i64 sprite;
    i32 talk_count;
} Sign;

// Must never contain unstable references, because reloads are just a memcpy.
typedef struct {
    MoveDirection input_move;
    iv2           hannah_pos_lead_prev; // kinda crazy, but needed for moving platform decisions
    bool          hannah_manual_moved_this_cycle;
    u8            logic_data[LOGIC_DATA_SIZE];
    Entity        platforms[PLATFORMS_MAX];
    i32           platforms_len;
    i32           cycle_index;

    Entity        cars[CARS_MAX];
    i32           cars_len;

    Button        buttons[BUTTONS_MAX];
    i32           buttons_len;

    Gate          gates[GATES_MAX];
    i32           gates_len;
    bool          permagates_open[PERMAGATES_MAX];

    Sign          signs[SIGNS_MAX];
    i32           signs_len;

    i32           level_index;
    i32           level_index_prev;
    v2            level_prev_offset_pos;

    // Level egg state
    iv2           egg_pos;
    bool          egg_exists;
    i32           egg_index;
    f32           egg_t;
    EggState      egg_states[2];

    union {
        Entity marchers[MARCHERS_MAX]; // ducks and hannah
        struct {
            // Hannah must be right before ducks.
            Entity hannah;
            Entity ducks[DUCKS_MAX];
        };
    };
    i32 marchers_len;
} LevelState;

typedef struct {
    // High level state
    GameMode         mode;
    GameMode         queued_game_mode;
    World*           world;
    World            world_copy;
    LevelState       state;
    LevelState       saved_state;

    i32              frames_since_init;
    f64              time;
    i32              cycle_stage_counter;
    bool             new_cycle_this_frame;
    bool             half_cycle_this_frame;
	InputButtonState input_buttons[BUTTON_COUNT];
	bool             mute;

	// Debugging
	bool             debug_stepping;
	i32              new_cycle_queued;

    // Misc mode data
    f32              level_reset_t;
    f32              transition_t;
    CutsceneMode     cutscene_mode;
    bool             god_mode;

    // Dialogue
    String           msg[MSG_LEN_MAX];
    i32              msg_len;
    i32              msg_string_cur;
    i32              msg_char_cur;
    f32              msg_char_t;
    bool             msg_speeding;
    f32              msg_char_pitch;
    i32              msg_sound_type;
    i64              msg_portrait;

    // Editor modes
    Editor           editor;
    u8               world_viewer_mode;
    i32              world_viewer_level_select_index;
    iv2              world_viewer_place_offset;
} Game;

#include "music.c"
#include "game_common.c"
#include "draw_game.c"
#include "mode_menu.c"
#include "mode_world_viewer.c"
#include "mode_dialogue.c"
#include "frog.c"
#include "mode_cinematic.c"
#include "mode_game.c"
#include "mode_menu_to_game.c"
#include "mode_level_reset.c"
#include "mode_egg_explode.c"
#include "editor.c"
#include "level_logic.c"

GAME_INIT(game_init) {
	Game* game = (Game*)game_memory;
	memset(game, 0, sizeof(Game));
	asset_pack_data = asset_memory;

//#ifndef __EMSCRIPTEN__
	// RELEASE: this is for live editing only
	World* world = world_asset(asset_pack_data, WORLD_MAIN);
	memcpy(&game->world_copy, world, sizeof(World));
	game->world = &game->world_copy;
//#endif

    for(i32 i = 0; i < LEVELS_MAX; i++) {
        Level* level = &game->world->levels[i];
        for(i32 j = 0; j < 64; j++) {
        }
    }

	game->new_cycle_this_frame = true;
	game->state.level_index = 3;
	game->state.level_index = 14;
	game->mode = MODE_GAME;
	game->mute = true;

	// setup entities. later, there will be structs for each type that reference
	// entity indices. A subservient tool.
	LevelState* state = &game->state;
	state->marchers_len = STARTING_DUCKS + 1;
	for(i32 i = 0; i < state->marchers_len; i++) {
    	Entity* entity = &state->marchers[i];
    	entity->pos_cur = iv2_new(2 - i, 0);
    	entity->pos_prev = iv2_new(2 - i - 1, 0);
    	entity->pos_lead = entity->pos_prev;
    	entity->pos_visible = v2_from_iv2(entity->pos_cur);
    	entity->pos_prev_visible = v2_from_iv2(entity->pos_prev);
    	if(i == 0) {
        	entity->sprite_handle = SPRITE_HANNAH_RIGHT;
    	} else {
        	entity->sprite_handle = SPRITE_DUCK_RIGHT;
    	}
    	entity->anim_offset_t = (i % 2);
	}
    assert(state->input_move == MOVE_NONE);

	// Backup state. later, this will happen every time we enter a level.
	game->saved_state = game->state;
}

GAME_UPDATE(game_update) {
	Game* game = (Game*)game_memory;
	Stack frame_stack = stack_from_memory(game_memory + sizeof(Game), MEGABYTE / 2, string_const("GameFrame"));

	dt *= debug_timescale;

	input_clear_buttons(game->input_buttons, BUTTON_COUNT);
	input_release_buttons_if_window_defocused(events, events_len, game->input_buttons, BUTTON_COUNT);
	game->input_buttons[BUTTON_LEFT]         = input_update_key_button(events, events_len, game->input_buttons[BUTTON_LEFT],         KEYCODE_A);
	game->input_buttons[BUTTON_RIGHT]        = input_update_key_button(events, events_len, game->input_buttons[BUTTON_RIGHT],        KEYCODE_D);
	game->input_buttons[BUTTON_UP]           = input_update_key_button(events, events_len, game->input_buttons[BUTTON_UP],           KEYCODE_W);
	game->input_buttons[BUTTON_DOWN]         = input_update_key_button(events, events_len, game->input_buttons[BUTTON_DOWN],         KEYCODE_S);
	game->input_buttons[BUTTON_START]        = input_update_key_button(events, events_len, game->input_buttons[BUTTON_START],        KEYCODE_SPACE);
	game->input_buttons[BUTTON_LEAVE]        = input_update_key_button(events, events_len, game->input_buttons[BUTTON_LEAVE],        KEYCODE_E);
	game->input_buttons[BUTTON_BECKON]       = input_update_key_button(events, events_len, game->input_buttons[BUTTON_BECKON],       KEYCODE_Q);
	game->input_buttons[BUTTON_RESET]        = input_update_key_button(events, events_len, game->input_buttons[BUTTON_RESET],        KEYCODE_R);
	game->input_buttons[BUTTON_MUTE]         = input_update_key_button(events, events_len, game->input_buttons[BUTTON_MUTE],         KEYCODE_M);
	game->input_buttons[BUTTON_QUIT]         = input_update_key_button(events, events_len, game->input_buttons[BUTTON_QUIT],         KEYCODE_ESCAPE);
	game->input_buttons[BUTTON_GOD]          = input_update_key_button(events, events_len, game->input_buttons[BUTTON_GOD],          KEYCODE_G);
	game->input_buttons[BUTTON_EDITOR]       = input_update_key_button(events, events_len, game->input_buttons[BUTTON_EDITOR],       KEYCODE_TAB);
	game->input_buttons[BUTTON_EDITOR_PLACE] = input_update_key_button(events, events_len, game->input_buttons[BUTTON_EDITOR_PLACE], KEYCODE_SPACE);

	if(input_button_pressed(game->input_buttons[BUTTON_MUTE])) {
    	game->mute = !game->mute;
	}

	AudioWaveChannel* wave = &audio->wave_channels[3];
	wave->amp = 0.0;

	game->debug_stepping = false;
	//game->state.level_index = 2;

    pre_update_level_logic(game, &frame_stack);
	switch(game->mode) {
    	case MODE_PREMENU: {
        	mode_premenu_to_menu_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_MENU: {
        	mode_menu_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_MENU_TO_GAME: {
        	mode_menu_to_game_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_GAME: {
        	mode_game_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_LEVEL_RESET: {
        	mode_level_reset_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_LEVEL_SWITCH: {
        	mode_level_switch_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_MSG: {
        	mode_msg_update(game, draw_list, audio, dt, &frame_stack);
    	} break;
    	case MODE_CUTSCENE: {
        	mode_cutscene_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_EGG_EXPLODE: {
        	mode_egg_explode_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_EGG_COLLECT: {
        	mode_egg_collect_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_EDITOR: {
        	mode_editor_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_WORLD_VIEWER: {
        	mode_world_viewer_update(game, draw_list, audio, dt, &frame_stack);
    	} break;
    	default: panic();
	}
    post_update_level_logic(game);

    if(game->mode != MODE_PREMENU) {
        update_music_state(game, audio, dt);
    }
	game->frames_since_init++;
}
