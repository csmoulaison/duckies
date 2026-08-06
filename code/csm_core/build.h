#ifndef csm_build_h_INCLUDED
#define csm_build_h_INCLUDED

#define BUILD_COMMON_PREFIX "gcc -std=c99 -I code/ -I extern/ "
#define BUILD_RELEASE_FLAGS " "
#define BUILD_DEBUG_FLAGS   "-g -rdynamic "
#define BUILD_WARNING_FLAGS "-Wall -Werror -Wno-unused "
#define BUILD_GL3W_FLAGS    "extern/GL/gl3w.c "
#define BUILD_X11_FLAGS     "-lX11 -lX11-xcb -lGL -lm -lxcb -lXfixes "
#define BUILD_MATH_FLAGS    "-lm "
#define BUILD_ALSA_FLAGS    "-lasound "

#define BUILD_FLAG_RELEASE    1 << 0
#define BUILD_FLAG_DEBUG      1 << 1
#define BUILD_FLAG_WARNINGS   1 << 2
#define BUILD_FLAG_LINK_GL3W  1 << 3
#define BUILD_FLAG_TARGET_X11 1 << 4
#define BUILD_FLAG_LINK_MATH  1 << 5
#define BUILD_FLAG_LINK_ALSA  1 << 6

#define BUILD_SYSTEM_CALL(call) { system_call_result = system(call); if(system_call_result != 0) { printf("build error: system_call failed on line %i\n", __LINE__); exit(1); } }

typedef enum {
    BUILD_RESULT_SUCCESS,
    BUILD_RESULT_ERROR
} BuildResult;

BuildResult build_static(String name, String main_path, u64 flags, Stack* stack);
BuildResult build_dynamic(String name, String main_path, u64 flags, Stack* stack);

#ifdef CSM_IMPLEMENTATION

BuildResult build_static(String name, String main_path, u64 flags, Stack* stack) {
    system("mkdir -p bin");
    system("mkdir -p build");
    // TODO: Shouldn't be hardcoded.
    system("ld -r -b binary build/asset/pack.data -o build/asset/pack.o");

    String cmd = string_from_stack(stack, 8196);
    string_cat(&cmd, string_const(BUILD_COMMON_PREFIX));

    if(flags & BUILD_FLAG_RELEASE) 
        string_cat(&cmd, string_const(BUILD_RELEASE_FLAGS));
    if(flags & BUILD_FLAG_DEBUG) 
        string_cat(&cmd, string_const(BUILD_DEBUG_FLAGS));
    if(flags & BUILD_FLAG_WARNINGS) 
        string_cat(&cmd, string_const(BUILD_WARNING_FLAGS));
    if(flags & BUILD_FLAG_LINK_GL3W) 
        string_cat(&cmd, string_const(BUILD_GL3W_FLAGS));
    if(flags & BUILD_FLAG_LINK_ALSA) 
        string_cat(&cmd, string_const(BUILD_ALSA_FLAGS));

    if(flags & BUILD_FLAG_TARGET_X11) {
        string_cat(&cmd, string_const(BUILD_X11_FLAGS));
    } else if(flags & BUILD_FLAG_LINK_MATH) {
        string_cat(&cmd, string_const(BUILD_MATH_FLAGS));
    }

    string_cat(&cmd, main_path);
    // TODO: Shouldn't be hardcoded.
    string_cat(&cmd, string_const(" build/asset/pack.o"));
    string_cat(&cmd, string_const(" -o bin/"));
    string_cat(&cmd, name);

    string_write_null_terminator(&cmd);
    if(system(cmd.text) != 0) {
        return BUILD_RESULT_ERROR;
    }

    return BUILD_RESULT_SUCCESS;
}

BuildResult build_dynamic(String name, String main_path, u64 flags, Stack* stack) {
    system("mkdir -p bin");
    system("mkdir -p build");

    String prog_o = string_from_stack(stack, 4096);
    string_cat(&prog_o, name);
    string_cat(&prog_o, string_const(".o"));

    String prog_tmp_so = string_from_stack(stack, 4096);
    string_cat(&prog_tmp_so, name);
    string_cat(&prog_tmp_so, string_const("_tmp.so"));

    String cmd = string_from_stack(stack, 8196);
    string_cat(&cmd, string_const(BUILD_COMMON_PREFIX));
    string_cat(&cmd, string_const("-c -fPIC -o build/"));
    string_cat(&cmd, prog_o);
    string_cat(&cmd, string_const(" "));
    string_cat(&cmd, main_path);
    string_write_null_terminator(&cmd);
    if(system(cmd.text) != 0) {
        return BUILD_RESULT_ERROR;
    }

    string_clear(&cmd);
    string_cat(&cmd, string_const(BUILD_COMMON_PREFIX));
    string_cat(&cmd, string_const("-shared build/"));
    string_cat(&cmd, prog_o);
    string_cat(&cmd, string_const(" -o bin/"));
    string_cat(&cmd, prog_tmp_so);
    string_cat(&cmd, string_const(" -lm"));
    string_write_null_terminator(&cmd);
    if(system(cmd.text) != 0) {
        return BUILD_RESULT_ERROR;
    }

    string_clear(&cmd);
    string_cat(&cmd, string_const("mv bin/"));
    string_cat(&cmd, prog_tmp_so);
    string_cat(&cmd, string_const(" bin/"));
    string_cat(&cmd, name);
    string_cat(&cmd, string_const(".so"));
    string_write_null_terminator(&cmd);
    if(system(cmd.text) != 0) {
        return BUILD_RESULT_ERROR;
    }

    // TODO:
    // -rm bin/prog_*
    return BUILD_RESULT_SUCCESS;
}

#endif
#endif
