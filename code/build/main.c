#define CSM_IMPLEMENTATION

#define CSM_MODULE_MATH
#define CSM_MODULE_ASSET_BUILDER
#define CSM_MODULE_BUILD
#define CSM_MODULE_MESH
#define CSM_MODULE_FONT
#define CSM_MODULE_TEXTURE
#define CSM_MODULE_SPRITE
#define CSM_MODULE_SPRITE_ATLAS
#define CSM_MODULE_ASEPRITE

#define BUFFER_TRACKING 1
#define BUFFER_BOUNDS_CHECKING 1
#define DEBUG_STACK 0

#include "csm_core/core.h"

#include "world.c"

#define PALETTE_COLOR_COUNT 16
#define PALETTE_COUNT       4
#define PALETTE_DATA_SIZE   (PALETTE_COLOR_COUNT * PALETTE_COUNT)
u32 palette_colors[PALETTE_COLOR_COUNT] = {
    0x00000000, //  0: Clear
    0xff331133, //  1: Purple (eyes, black)
    0xff88aaff, //  2: Orange (hair, bill)
    0xffc4d4f4, //  3: Peach  (skin)
    0xffee8866, //  4: Blue   (sweater)
    0xff88ffff, //  5: Lellow (duck)
    0xff66ff88, //  6: Green  (grass)
    0xff33dd55, //  7: Dark G. (grass2)
    0xff000000, //  8: Black
    0xffFFFFFF, //  9: White
    0xffF300FF, // 10: undef
    0xffF300FF, // 11: undef
    0xffF300FF, // 12: undef
    0xffF300FF, // 13: undef
    0xffF300FF, // 14: undef
    0xffF300FF, // 15: undef
};
u32 palette_indices[PALETTE_DATA_SIZE] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, // reg
    0,  8,  1,  2,  1,  2,  7,  8,  1,  1, 1,  1,  1,  1,  1,  1,  // dark
    0,  8,  8,  8,  8,  8,  8,  8,  8,  8, 8,  8,  8,  8,  8,  8,  // black
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, // all clear
};

i32 main(i32 argc, char** argv) {
    system("mkdir -p bin");
    system("mkdir -p build");
    system("mkdir -p build/asset");

    // Allocate memory hierarchy.
    void* root_mem = malloc(MEGABYTE * 128);
    assert(root_mem != NULL);
    Stack root_stack = stack_from_memory(root_mem, MEGABYTE * 128, string_const("Root"));
    Stack asset_builder_stack = stack_from_stack(&root_stack, MEGABYTE * 32, string_const("Builder"));
    Stack atlas_builder_stack = stack_from_stack(&root_stack, MEGABYTE * 32, string_const("Builder"));
    Stack scratch_stack = stack_from_stack(&root_stack, MEGABYTE * 64, string_const("Scratch"));

    // Build asset pack and header.
    AssetBuilder* builder = (AssetBuilder*)stack_alloc(&scratch_stack, sizeof(AssetBuilder));
    asset_builder_init(builder, &asset_builder_stack);

    // - Sprite atlas and texture
    SpriteAtlasBuilder* atlas_builder = (SpriteAtlasBuilder*)stack_alloc(&scratch_stack, sizeof(SpriteAtlasBuilder));
    sprite_atlas_builder_init(atlas_builder, TEXTURE_FORMAT_8_BIT_PALLETIZED, &atlas_builder_stack);
    aseprite_directory_to_sprite_atlas(string_const("assets/aseprite/"), string_const("assets/bmp/"), atlas_builder, &scratch_stack);
    sprite_atlas_build_assets(atlas_builder, builder, &scratch_stack);

    // - Palette texture
    u32 palette_data[PALETTE_DATA_SIZE];
    for(i32 i = 0; i < PALETTE_DATA_SIZE; i++) {
        palette_data[i] = palette_colors[palette_indices[i]];
    }
    TextureData* palette = (TextureData*)stack_alloc(&scratch_stack, texture_size_from_dimensions(PALETTE_DATA_SIZE, 1, TEXTURE_FORMAT_RGBA));
    palette->width = PALETTE_DATA_SIZE;
    palette->height = 1;
    palette->format = TEXTURE_FORMAT_RGBA;
    memcpy(palette->pixel_buffer, palette_data, PALETTE_DATA_SIZE * 4);
    asset_builder_push_asset(builder, 
        string_const("PALETTE"), string_const("TEXTURE"), string_const("TextureData"),
        palette, texture_size(palette));

    // - Primitive quad
    Primitive2dData* primitive = primitive_2d_from_data(primitive_2d_quad_vertices, PRIMITIVE_2D_QUAD_VERTICES_LEN, &scratch_stack);
    asset_builder_push_asset(builder, 
        string_const("QUAD"), string_const("PRIMITIVE_2D"), string_const("Primitive2dData"),
        primitive, primitive_2d_size(primitive));

    // - World (level) data
    File world_file;
    World* world;
    if(file_try_open(string_const(WORLD_PATH_FROM_BUILD), FILE_OPEN_READ, &world_file)) {
        world = world_from_file(&world_file, &scratch_stack);
    } else {
        printf("could not load world!\n");
        world = (World*)stack_alloc(&scratch_stack, sizeof(World));
    }
    asset_builder_push_asset(builder, 
        string_const("MAIN"), string_const("WORLD"), string_const("World"),
        world, sizeof(World));

    asset_builder_output_source(builder, string_const("code/generated/asset_handles.c"), string_const("code/generated/asset_data.c"));
    asset_builder_output_pack(builder, string_const("build/asset/pack.data"));

    // Execute build commands.
    String program_name = string_const("jam");
    if(argc > 1 && strcmp(argv[1], "web") == 0) {
        system("source extern/emsdk/emsdk_env.sh && \
            emcc code/emscripten.c -o bin/main.html -sUSE_SDL=2 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1 \
            --embed-file build/asset --embed-file code/shaders@/shaders \
            -Wall -Werror -Wno-unused -Wno-format -g -gsource-map -fsanitize=undefined -sASSERTIONS=2 -sSAFE_HEAP=1 -sALLOW_TABLE_GROWTH -sSTACK_SIZE=2000000 --emrun");
    } else {
        u64 flags = BUILD_FLAG_DEBUG | BUILD_FLAG_WARNINGS | BUILD_FLAG_LINK_GL3W | BUILD_FLAG_TARGET_X11 | BUILD_FLAG_LINK_ALSA;
        bool dynamic_only = (argc > 1 && strcmp(argv[1], "dynamic") == 0);
        if(!dynamic_only) {
            if(build_static(program_name, string_const("code/main.c"), flags, &scratch_stack) != BUILD_RESULT_SUCCESS) {
                goto error;
            }
        }
        if(build_dynamic(program_name, string_const("code/game.c"), flags, &scratch_stack) != BUILD_RESULT_SUCCESS) {
            goto error;
        }
    }

    // Copy shaders
    system("mkdir -p bin/shaders");
    system("cp -r code/shaders/ bin/");

    return 0;

error:
    fprintf(stderr, "Dynamic build command unsuccessful.\n");
    panic();
}
