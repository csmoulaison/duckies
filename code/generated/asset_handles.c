// Pregenerated file. Any changes made will be erased on recompilation.

#define TEXTURE_COUNT 2
#define TEXTURE_SPRITE_ATLAS 0
#define TEXTURE_PALETTE 1

#define SPRITE_COUNT 16
#define SPRITE_LILY 0
#define SPRITE_HANNAH_LEFT 1
#define SPRITE_NUMS 2
#define SPRITE_DUCK_JUMP 3
#define SPRITE_DUCK_LEFT 4
#define SPRITE_HAPPY_MASK 5
#define SPRITE_DUCK_THUMBS_UP 6
#define SPRITE_MAN 7
#define SPRITE_HANNAH_FRONT 8
#define SPRITE_HANNAH_UP 9
#define SPRITE_WATER 10
#define SPRITE_DUCK_RIGHT 11
#define SPRITE_GRASS 12
#define SPRITE_CURSOR 13
#define SPRITE_HANNAH_DOWN 14
#define SPRITE_HANNAH_RIGHT 15

#define PRIMITIVE_2D_COUNT 1
#define PRIMITIVE_2D_QUAD 0

#define WORLD_COUNT 1
#define WORLD_MAIN 0

u64 texture_data_offsets[TEXTURE_COUNT] = {
    0,
    67296
};

TextureData* texture_asset(char* pack, u64 handle) {
    return (TextureData*)&pack[texture_data_offsets[handle]];
}

u64 sprite_data_offsets[SPRITE_COUNT] = {
    65568,
    65632,
    65760,
    65936,
    66048,
    66176,
    66256,
    66352,
    66464,
    66592,
    66720,
    66784,
    66912,
    66976,
    67040,
    67168
};

SpriteData* sprite_asset(char* pack, u64 handle) {
    return (SpriteData*)&pack[sprite_data_offsets[handle]];
}

u64 primitive_2d_data_offsets[PRIMITIVE_2D_COUNT] = {
    67584
};

Primitive2dData* primitive_2d_asset(char* pack, u64 handle) {
    return (Primitive2dData*)&pack[primitive_2d_data_offsets[handle]];
}

u64 world_data_offsets[WORLD_COUNT] = {
    67648
};

World* world_asset(char* pack, u64 handle) {
    return (World*)&pack[world_data_offsets[handle]];
}

