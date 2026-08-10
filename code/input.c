#define INPUT_DOWN_BIT     0b00000001
#define INPUT_PRESSED_BIT  0b00000010
#define INPUT_RELEASED_BIT 0b00000100

typedef u8 InputButtonState;

typedef enum {
	BUTTON_UP,
	BUTTON_LEFT,
	BUTTON_DOWN,
	BUTTON_RIGHT,
	BUTTON_START,
	BUTTON_LEAVE,
	BUTTON_BECKON,
	BUTTON_RESET,
	BUTTON_MUTE,
	BUTTON_QUIT,
    // RELEASE: Disable refs to these
	BUTTON_EDITOR,
	BUTTON_EDITOR_PLACE,
	BUTTON_COUNT
} InputButton;

bool input_button_down(InputButtonState button) {
	return button & INPUT_DOWN_BIT;
}

bool input_button_pressed(InputButtonState button) {
	return button & INPUT_PRESSED_BIT;
}

bool input_button_released(InputButtonState button) {
	return button & INPUT_RELEASED_BIT;
}

void input_release_buttons_if_window_defocused(WindowEvent* events, i32 events_len, InputButtonState* buttons, i32 buttons_len) {
	for(i32 i = 0; i < events_len; i++) {
		if(events[i].type == WINDOW_EVENT_DEFOCUS) {
            for(i32 j = 0; j < buttons_len; j++) {
        		if(buttons[j] & INPUT_DOWN_BIT) {
        			buttons[j] = INPUT_RELEASED_BIT;
        		}
            }
            break;
		}
	}
}

void input_clear_buttons(InputButtonState* buttons, i32 len) {
	for(i32 i = 0; i < len; i++) {
		buttons[i] = buttons[i] & ~INPUT_PRESSED_BIT & ~INPUT_RELEASED_BIT;
	}
}

InputButtonState input_update_key_button(WindowEvent* events, i32 events_len, InputButtonState button, Keycode key) {
	for(i32 i = 0; i < events_len; i++) {
		WindowEvent* event = &events[i];
		if(event->keycode == key) {
			if(event->type == WINDOW_EVENT_KEYDOWN) {
				if(button & INPUT_DOWN_BIT) {
					break;
				}
				button = button | INPUT_DOWN_BIT | INPUT_PRESSED_BIT;
			} else if(event->type == WINDOW_EVENT_KEYUP) {
				if(button & INPUT_DOWN_BIT) {
					button = INPUT_RELEASED_BIT;
				}
			}
		}
	}
	return button;
}
