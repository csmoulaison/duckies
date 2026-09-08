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

#include "debug_overlay.c"

#define DUCKS_MAX           16
#define MARCHERS_MAX        1 + DUCKS_MAX
#define PLATFORMS_MAX       64
#define CARS_MAX            32
#define BUTTONS_MAX         16
#define GATES_MAX           16
#define TRIGGER_BUTTONS_MAX 16
#define PERMAGATES_MAX      64
#define SIGNS_MAX           8

#define TIME_SCALE          2.0
#define MOVE_QUEUE_MAX      64
#define LOGIC_DATA_SIZE     256
#define MOVE_CHAIN_SIZE     256
#define MSG_LEN_MAX         32

#define STARTING_DUCKS      0

f32 dt_mod = 1.0;

typedef enum {
    MOVE_NONE  = 0,
    MOVE_UP    = 1,
    MOVE_LEFT  = 2,
    MOVE_DOWN  = 3,
    MOVE_RIGHT = 4
} MoveDirection;

typedef enum {
    PLATFORM_FLOAT,
    PLATFORM_FAKE_WARN,
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
    // honk state
    bool              honk_this_cycle;
    // misc state
    PlatformSinkState sink_state;
    i32               car_width;
    i32               palette_swap;
    i32               snake_index;
    i32               snake_frame_root;
    i32               reappear_countdown;
    // crumble state
    bool              crumbler;
    i32               max_health;
    i32               hits_taken;
    f32               transition_t;
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
    MODE_WORLD_VIEWER,
    MODE_APPEASE_TROLL,
    MODE_GATE_TO_TEA
} GameMode;

typedef enum {
    CUT_NONE,
    CUT_OPENING,
    CUT_EGG_RUMBLE,
    CUT_FROG_PAUSE,
    CUT_FROG_INVITATION,
    CUT_FROG_INVITATION_2,
    CUT_RIVER_GATE,
    CUT_TEA_PARTY
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
    TRIGGER_BUTTONS,
    TRIGGER_REMOTE
} GateTrigger;

typedef struct {
    GateState   state;
    iv2         pos;
    f32         transition_t;
    i32         permagate_index;

    GateTrigger trigger_type;
    union {
        struct {
            u8 indices[TRIGGER_BUTTONS_MAX];
            u8 len;
        } buttons;
    } trigger;
} Gate;

typedef struct {
    i32 index;
    iv2 pos;
    i64 sprite;
    i32 talk_count;
    bool doublewide;
} Sign;

typedef enum {
    TIME_EVENT_NONE = 0,
    TIME_EVENT_BUTTON_GATE
} TimeEventType;

typedef struct {
    TimeEventType type;
    i32           cycles;
    union {
        struct {
            u8 button_indices[TRIGGER_BUTTONS_MAX];
            u8 buttons_len;
            u8 permagate_index;
        } button_gate;
    };
} TimeEvent;

typedef enum {
    MUFFIN_GAME_INACTIVE,
    MUFFIN_GAME_ACTIVE,
    MUFFIN_GAME_LOST,
    MUFFIN_GAME_WON
} MuffinGameMode;

typedef enum {
    MUFFIN_DOWN,
    MUFFIN_WARN,
    MUFFIN_UP,
    MUFFIN_ASLEEP
} MuffinState;

typedef struct {
    MuffinState state;
    i32         state_counter; 
    f32         transition_t;
    i32         sleep_count;
} Muffin;

typedef struct {
    MuffinGameMode mode;
    i32            cycle;
    Muffin         muffins[9];
    i32            spawn_cooldown;
} MuffinGame;

typedef enum {
    SNAKE_GAME_INACTIVE,
    SNAKE_GAME_ACTIVE,
    SNAKE_GAME_LOST,
    SNAKE_GAME_WON
} SnakeGameMode;

typedef struct {
    SnakeGameMode mode;
    i32           snake_extra_length;
    iv2           apple_pos;
    bool          apple_active;
    f32           apple_transition_t;
} SnakeGame;

// Must never contain unstable references, because reloads are just a memcpy.
typedef struct {
    MoveDirection input_move;
    bool          input_honk;
    iv2           hannah_pos_lead_prev; // kinda crazy, but needed for moving platform decisions
    bool          hannah_manual_moved_this_cycle;
    u8            logic_data[LOGIC_DATA_SIZE];

    // Non marching entities
    Entity        platforms[PLATFORMS_MAX];
    i32           platforms_len;

    Entity        cars[CARS_MAX];
    i32           cars_len;

    Entity        frogo;
    bool          frogo_exists;
    bool          frogo_alerted;
    bool          frogo_alerted_just_now;
    MoveDirection frogo_alert_direction;

    // Bespoke objects
    iv2           egg_pos;
    bool          egg_exists;
    i32           egg_index;
    f32           egg_t;
    EggState      egg_states[16];

    Button        buttons[BUTTONS_MAX];
    i32           buttons_len;

    Gate          gates[GATES_MAX];
    i32           gates_len;
    bool          permagates_open[PERMAGATES_MAX];

    Sign          signs[SIGNS_MAX];
    i32           signs_len;

    MuffinGame    muffin_game;
    SnakeGame     snake_game;

    // Random state
    bool          boat_activated;
    bool          boat_riding;
    f32           large_gate_t;
    bool          honking_unlocked;
    bool          frogo_appeased;
    bool          troll_appeased;

    // Draw override
    bool          override_egg_draw;
    bool          override_hannah_draw;

	// Time event
	TimeEvent     queued_time_event;
	TimeEvent     active_time_event;

    // Level
    i32           level_index;
    i32           level_index_prev;
    v2            level_prev_offset_pos;

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

typedef enum {
    MUSIC_OVERRIDE_NONE,
    MUSIC_OVERRIDE_MENU_FADE,
    MUSIC_OVERRIDE_WINDY,
    MUSIC_OVERRIDE_WINDY_DRUMS
} MusicOverrideState;

typedef enum {
    MSG_FROG,
    MSG_FROGO,
    MSG_DUCK,
    MSG_SIGN,
    MSG_TROLL,
    MSG_MUFFIN,
    MSG_SNAKE,
    MSG_HANNAH
} MsgType;

typedef struct {
    MsgType type;
    String  s;
} Msg;

typedef struct {
    // High level state
    GameMode         mode;
    GameMode         queued_game_mode;
    World*           world;
    World            world_copy;
    LevelState       state;
    LevelState       saved_state;
    MusicOverrideState music_override_state;

    i32              frames_since_init;
    f64              time;
    i32              cycle_stage_counter;
    i32              cycle_index;
    bool             new_cycle_this_frame;
    bool             half_cycle_this_frame;
	InputButtonState input_buttons[BUTTON_COUNT];
	bool             mute;

	// Debugging
	bool             debug_stepping;
	i32              new_cycle_queued;
	char             dbg_buf[256];
	String           dbg_str;

    // Misc mode data
    f32              level_reset_t;
    f32              transition_t;
    CutsceneMode     cutscene_mode;
    bool             god_mode;
    bool             first_move_made;

    // Dialogue
    Msg              msg_chain[MSG_LEN_MAX];
    i32              msg_chain_len;
    i32              msg_string_cur;
    i32              msg_char_cur;
    f32              msg_char_t;
    bool             msg_speeding;
    f32              msg_char_pitch;

    // Editor modes
    Editor           editor;
    u8               world_viewer_mode;
    i32              world_viewer_level_select_index;
    iv2              world_viewer_place_offset;
} Game;

#include "game_common.c"
#include "music.c"
#include "draw_game.c"
#include "mode_menu.c"
#include "mode_world_viewer.c"
#include "mode_dialogue.c"
#include "muffin_sprout.c"
#include "snake_game.c"
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

	// RELEASE: this is for live editing only
	World* world = world_asset(asset_pack_data, WORLD_MAIN);
	memcpy(&game->world_copy, world, sizeof(World));
	game->world = &game->world_copy;

    // RELEASE: get rid of.
    game->dbg_str = init_dbg_overlay(game->dbg_buf);

    for(i32 i = 0; i < LEVELS_MAX; i++) {
        Level* level = &game->world->levels[i];
        for(i32 j = 0; j < 64; j++) {

        }
    }

	game->new_cycle_this_frame = true;
	game->state.level_index = 3;
	game->mode = MODE_MENU;
	// testing overrides
	//game->state.level_index = 35;
	//game->mode = MODE_GAME;


	// setup entities. later, there will be structs for each type that reference
	// entity indices. A subservient tool.
	LevelState* state = &game->state;
	state->marchers_len = STARTING_DUCKS + 1;
	for(i32 i = 0; i < state->marchers_len; i++) {
    	Entity* entity = &state->marchers[i];
    	entity->pos_cur = iv2_new(4 - i, 4);
    	entity->pos_prev = iv2_new(4 - i - 1, 4);
    	entity->pos_lead = entity->pos_prev;
    	entity->pos_visible = v2_from_iv2(entity->pos_cur);
    	entity->pos_prev_visible = v2_from_iv2(entity->pos_prev);
    	entity->pos_t = 1.0;
    	if(i == 0) {
        	entity->sprite_handle = SPRITE_HANNAH_LEFT;
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

	dt *= dt_mod;

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

    if(game->mode == MODE_APPEASE_TROLL) {
        game->state.troll_appeased = true;
        game->mode = MODE_GAME;
    }

    pre_update_level_logic(game, &frame_stack, dt);
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
    	case MODE_GATE_TO_TEA: {
        	mode_gate_to_tea_update(game, draw_list, audio, dt);
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

	// RELEASE: get rid of
	dbg_draw(&game->dbg_str, draw_list);
}
