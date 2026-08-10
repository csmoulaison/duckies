// Pregenerated file. Any changes made will be erased on recompilation.

#define TEXTURE_COUNT 2
#define TEXTURE_SPRITE_ATLAS 0
#define TEXTURE_PALETTE 1

#define SPRITE_COUNT 31
#define SPRITE_DUCK_EXPLODE 0
#define SPRITE_FROG 1
#define SPRITE_FONT_SMALL 2
#define SPRITE_LILY 3
#define SPRITE_HANNAH_LEFT 4
#define SPRITE_NUMS 5
#define SPRITE_DUCK_JUMP 6
#define SPRITE_DUCK_PORTRAIT 7
#define SPRITE_DUCK_LEFT 8
#define SPRITE_SNAKE_PORTRAIT 9
#define SPRITE_DEBUG_CIRCLE 10
#define SPRITE_SPRITE_MAN_PORTRAIT 11
#define SPRITE_CLIFF 12
#define SPRITE_HAPPY_MASK 13
#define SPRITE_DUCK_THUMBS_UP 14
#define SPRITE_FROG_PORTRAIT 15
#define SPRITE_CAR_A 16
#define SPRITE_LILY_SINK 17
#define SPRITE_DIALOGUE_BOX 18
#define SPRITE_ROAD 19
#define SPRITE_FONT_BIG 20
#define SPRITE_MAN 21
#define SPRITE_HANNAH_FRONT 22
#define SPRITE_HANNAH_UP 23
#define SPRITE_WATER 24
#define SPRITE_DUCK_RIGHT 25
#define SPRITE_GRASS 26
#define SPRITE_CURSOR 27
#define SPRITE_HANNAH_DOWN 28
#define SPRITE_HANNAH_RIGHT 29
#define SPRITE_MAN_PORTRAIT 30

#define PRIMITIVE_2D_COUNT 1
#define PRIMITIVE_2D_QUAD 0

#define WORLD_COUNT 1
#define WORLD_MAIN 0

u64 texture_data_offsets[TEXTURE_COUNT] = {
    0,
    69856
};

TextureData* texture_asset(char* pack, u64 handle) {
    return (TextureData*)&pack[texture_data_offsets[handle]];
}

u64 sprite_data_offsets[SPRITE_COUNT] = {
    65568,
    65696,
    65760,
    66512,
    66592,
    66704,
    66880,
    66992,
    67040,
    67152,
    67200,
    67264,
    67344,
    67664,
    67744,
    67840,
    67888,
    67952,
    68016,
    68080,
    68144,
    68896,
    69008,
    69136,
    69248,
    69312,
    69424,
    69488,
    69552,
    69664,
    69776
};

SpriteData* sprite_asset(char* pack, u64 handle) {
    return (SpriteData*)&pack[sprite_data_offsets[handle]];
}

u64 primitive_2d_data_offsets[PRIMITIVE_2D_COUNT] = {
    70400
};

Primitive2dData* primitive_2d_asset(char* pack, u64 handle) {
    return (Primitive2dData*)&pack[primitive_2d_data_offsets[handle]];
}

u64 world_data_offsets[WORLD_COUNT] = {
    70464
};

World* world_asset(char* pack, u64 handle) {
    return (World*)&pack[world_data_offsets[handle]];
}

