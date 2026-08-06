#ifndef assert_h_INCLUDED
#define assert_h_INCLUDED

#ifndef __EMSCRIPTEN__
#include <execinfo.h>
#endif

#define DEBUG_ASSERTIONS 1
#define DEBUG_STRICT_ASSERTIONS 1

#define panic() do { printf("Panic at %s:%u\n", __FILE__, __LINE__); print_callstack(); exit(1); } while(0)

#undef assert
#define assert(assertion) do { if(!(assertion)) { printf("Assertion failed at %s:%u\n", __FILE__, __LINE__); print_callstack(); exit(1); } } while(0)

#if DEBUG_ASSERTIONS
	#define debug_assert(assertion) assert(assertion)
#else
	#define debug_assert
#endif

#if DEBUG_STRICT_ASSERTIONS
	#define strict_assert(assertion) assert(assertion)
#else
	#define strict_assert
#endif

#ifdef CSM_IMPLEMENTATION

void print_callstack() {
#ifndef __EMSCRIPTEN__
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    printf("--- Call Stack ---\n");
    for (int i = 0; i < frames; i++) {
        printf("%s\n", strs[i]);
    }
    free(strs); 
#endif
}

#endif
#endif // assert_h_INCLUDED
