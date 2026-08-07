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

#include "music.c"

#define DUCKS_MAX          16
#define MARCHERS_MAX       1 + DUCKS_MAX
#define LOGIC_ENTITIES_MAX 64

#define TIME_SCALE         1.25f
#define MOVE_QUEUE_MAX     64
#define LOGIC_DATA_SIZE    256
#define MOVE_CHAIN_SIZE    256

#define STARTING_DUCKS     1

typedef enum {
    MOVE_NONE = 0,
    MOVE_UP,
    MOVE_LEFT,
    MOVE_DOWN,
    MOVE_RIGHT,
} MoveDirection;

typedef enum {
    LOGIC_NONE = 0,
    LOGIC_MOVING_PLATFORM,
} LogicEntityType;

// Must never have an unstable reference
typedef struct {
    u64           sprite_handle;
    iv2           pos_cur;
    iv2           pos_prev;
    iv2           pos_lead;
    v2            pos_visible;
    v2            pos_prev_visible;
    f32           pos_t;
    f32           anim_offset_t;
    MoveDirection move_this_cycle;
    MoveDirection pulled_move_this_cycle;
    // Used by logic entities
    i32           logic_type;
} Entity;

typedef struct {
    i32  level_index;
    iv2  cursor_pos;
    Tile cursor_tile;
} Editor;

typedef enum {
    MODE_GAME = 0,
    MODE_LEVEL_RESET,
    MODE_EDITOR
} GameMode;

typedef struct {
    MoveDirection input_move;
    u8            logic_data[LOGIC_DATA_SIZE];
    Entity        logic_entities[LOGIC_ENTITIES_MAX];
    i32           logic_entities_len;
    i32           cycle_index;

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
    World*           world;
    World            world_copy;
    i32              level_index;
    LevelState       state;
    LevelState       saved_state;

    i32              frames_since_init;
    f64              time;
    i32              cycle_stage_counter;
    bool             new_cycle_this_frame;
    bool             half_cycle_this_frame;
	InputButtonState input_buttons[BUTTON_COUNT];

    // Level reset mode
    f32              level_reset_t;

    // Editor mode
    Editor           editor;
} Game;

#include "game_common.c"
#include "mode_game.c"
#include "mode_level_reset.c"
#include "editor.c"
#include "level_logic.c"

GAME_INIT(game_init) {
	Game* game = (Game*)game_memory;
	memset(game, 0, sizeof(Game));
	asset_pack_data = asset_memory;

#ifndef __EMSCRIPTEN__
	// RELEASE: this is for live editing only
	World* world = world_asset(asset_pack_data, WORLD_MAIN);
	memcpy(&game->world_copy, world, sizeof(World));
	game->world = &game->world_copy;
#endif

	// setup entities. later, there will be structs for each type that reference
	// entity indices. A subservient tool.
	LevelState* state = &game->state;
	state->marchers_len = STARTING_DUCKS + 2;
	for(i32 i = 0; i < state->marchers_len; i++) {
    	Entity* entity = &state->marchers[i];
    	entity->pos_cur = iv2_new(2 - i, 1);
    	entity->pos_prev = iv2_new(2 - i - 1, 1);
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

	input_clear_buttons(game->input_buttons, BUTTON_COUNT);
	input_release_buttons_if_window_defocused(events, events_len, game->input_buttons, BUTTON_COUNT);
	game->input_buttons[BUTTON_LEFT]         = input_update_key_button(events, events_len, game->input_buttons[BUTTON_LEFT],         KEYCODE_A);
	game->input_buttons[BUTTON_RIGHT]        = input_update_key_button(events, events_len, game->input_buttons[BUTTON_RIGHT],        KEYCODE_D);
	game->input_buttons[BUTTON_UP]           = input_update_key_button(events, events_len, game->input_buttons[BUTTON_UP],           KEYCODE_W);
	game->input_buttons[BUTTON_DOWN]         = input_update_key_button(events, events_len, game->input_buttons[BUTTON_DOWN],         KEYCODE_S);
	game->input_buttons[BUTTON_LEAVE]        = input_update_key_button(events, events_len, game->input_buttons[BUTTON_LEAVE],        KEYCODE_E);
	game->input_buttons[BUTTON_BECKON]       = input_update_key_button(events, events_len, game->input_buttons[BUTTON_BECKON],       KEYCODE_Q);
	game->input_buttons[BUTTON_RESET]        = input_update_key_button(events, events_len, game->input_buttons[BUTTON_RESET],        KEYCODE_R);
	game->input_buttons[BUTTON_QUIT]         = input_update_key_button(events, events_len, game->input_buttons[BUTTON_QUIT],         KEYCODE_ESCAPE);
	game->input_buttons[BUTTON_EDITOR]       = input_update_key_button(events, events_len, game->input_buttons[BUTTON_EDITOR],       KEYCODE_TAB);
	game->input_buttons[BUTTON_EDITOR_PLACE] = input_update_key_button(events, events_len, game->input_buttons[BUTTON_EDITOR_PLACE], KEYCODE_SPACE);

    pre_update_level_logic(game, &frame_stack);
	switch(game->mode) {
    	case MODE_GAME: {
        	mode_game_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_LEVEL_RESET: {
        	mode_level_reset_update(game, draw_list, audio, dt);
    	} break;
    	case MODE_EDITOR: {
        	mode_editor_update(game, draw_list, audio, dt);
    	} break;
    	default: panic();
	}
    post_update_level_logic(game);

    update_music_state(game, audio, dt);
	game->frames_since_init++;
}
