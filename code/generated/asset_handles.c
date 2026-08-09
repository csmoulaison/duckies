// Pregenerated file. Any changes made will be erased on recompilation.

#define TEXTURE_COUNT 2
#define TEXTURE_SPRITE_ATLAS 0
#define TEXTURE_PALETTE 1

#define SPRITE_COUNT 20
#define SPRITE_FONT_SMALL 0
#define SPRITE_LILY 1
#define SPRITE_HANNAH_LEFT 2
#define SPRITE_NUMS 3
#define SPRITE_DUCK_JUMP 4
#define SPRITE_DUCK_LEFT 5
#define SPRITE_DEBUG_CIRCLE 6
#define SPRITE_HAPPY_MASK 7
#define SPRITE_DUCK_THUMBS_UP 8
#define SPRITE_LILY_SINK 9
#define SPRITE_FONT_BIG 10
#define SPRITE_MAN 11
#define SPRITE_HANNAH_FRONT 12
#define SPRITE_HANNAH_UP 13
#define SPRITE_WATER 14
#define SPRITE_DUCK_RIGHT 15
#define SPRITE_GRASS 16
#define SPRITE_CURSOR 17
#define SPRITE_HANNAH_DOWN 18
#define SPRITE_HANNAH_RIGHT 19

#define PRIMITIVE_2D_COUNT 1
#define PRIMITIVE_2D_QUAD 0

#define WORLD_COUNT 1
#define WORLD_MAIN 0

u64 texture_data_offsets[TEXTURE_COUNT] = {
    0,
    68848
};

TextureData* texture_asset(char* pack, u64 handle) {
    return (TextureData*)&pack[texture_data_offsets[handle]];
}

u64 sprite_data_offsets[SPRITE_COUNT] = {
    65568,
    66320,
    66400,
    66512,
    66688,
    66800,
    66912,
    66976,
    67056,
    67152,
    67216,
    67968,
    68080,
    68208,
    68320,
    68384,
    68496,
    68560,
    68624,
    68736
};

SpriteData* sprite_asset(char* pack, u64 handle) {
    return (SpriteData*)&pack[sprite_data_offsets[handle]];
}

u64 primitive_2d_data_offsets[PRIMITIVE_2D_COUNT] = {
    69136
};

Primitive2dData* primitive_2d_asset(char* pack, u64 handle) {
    return (Primitive2dData*)&pack[primitive_2d_data_offsets[handle]];
}

u64 world_data_offsets[WORLD_COUNT] = {
    69200
};

World* world_asset(char* pack, u64 handle) {
    return (World*)&pack[world_data_offsets[handle]];
}

