#ifndef fiedler_h_INCLUDED
#define fiedler_h_INCLUDED

/* Usage:
	  
	f64 time;
	f64 accumulator;
	fiedler_init_time(&time, &accumulator);
	while(_game_running) {
		fiedler_accumulate_time(&time, &accumulator);
		while(accumulator >= _frame_length) {
			_logical_game_update(_game, _frame_length);
			accumulator -= _frame_length;
		}

	}

*/

#include <time.h>

#ifndef FIEDLER_MINIMUM_FRAME_TIME
#define FIEDLER_MINIMUM_FRAME_TIME 0.25f
#endif

void fiedler_init_time(f64* time, f64* accumulator);
void fiedler_accumulate_time(f64* time, f64* accumulator);

#ifdef CSM_IMPLEMENTATION

f64 time_seconds() {
	struct timespec current;
	assert(clock_gettime(CLOCK_REALTIME, &current) == 0);
	return (f64)current.tv_sec + (f32)current.tv_nsec / 1000000000.0f;
}

void fiedler_init_time(f64* time, f64* accumulator) {
	*time = time_seconds();
	*accumulator = 0.0f;
}

void fiedler_accumulate_time(f64* time, f64* accumulator) {
	f64 new_time = time_seconds();
	f64 frame_time = new_time - *time;
	if(frame_time > FIEDLER_MINIMUM_FRAME_TIME) {
		frame_time = FIEDLER_MINIMUM_FRAME_TIME;
	}
	*time = new_time;
	*accumulator += frame_time;
}

#endif // CSM_IMPLEMENTATION
#endif // fiedler_h_INCLUDED
