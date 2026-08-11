// Pregenerated file. Any changes made will be erased on recompilation.

#define TEXTURE_COUNT 2
#define TEXTURE_SPRITE_ATLAS 0
#define TEXTURE_PALETTE 1

#define SPRITE_COUNT 35
#define SPRITE_DUCK_EXPLODE 0
#define SPRITE_FROG 1
#define SPRITE_FONT_SMALL 2
#define SPRITE_LILY 3
#define SPRITE_BUTTON 4
#define SPRITE_HANNAH_LEFT 5
#define SPRITE_NUMS 6
#define SPRITE_DUCK_JUMP 7
#define SPRITE_DUCK_PORTRAIT 8
#define SPRITE_DUCK_LEFT 9
#define SPRITE_SNAKE_PORTRAIT 10
#define SPRITE_DEBUG_CIRCLE 11
#define SPRITE_SPRITE_MAN_PORTRAIT 12
#define SPRITE_CLIFF 13
#define SPRITE_PUFF 14
#define SPRITE_HAPPY_MASK 15
#define SPRITE_GATE 16
#define SPRITE_DUCK_THUMBS_UP 17
#define SPRITE_SIGN 18
#define SPRITE_FROG_PORTRAIT 19
#define SPRITE_CAR_A 20
#define SPRITE_LILY_SINK 21
#define SPRITE_DIALOGUE_BOX 22
#define SPRITE_ROAD 23
#define SPRITE_FONT_BIG 24
#define SPRITE_MAN 25
#define SPRITE_HANNAH_FRONT 26
#define SPRITE_HANNAH_UP 27
#define SPRITE_WATER 28
#define SPRITE_DUCK_RIGHT 29
#define SPRITE_GRASS 30
#define SPRITE_CURSOR 31
#define SPRITE_HANNAH_DOWN 32
#define SPRITE_HANNAH_RIGHT 33
#define SPRITE_MAN_PORTRAIT 34

#define PRIMITIVE_2D_COUNT 1
#define PRIMITIVE_2D_QUAD 0

#define WORLD_COUNT 1
#define WORLD_MAIN 0

u64 texture_data_offsets[TEXTURE_COUNT] = {
    0,
    70240
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
    66672,
    66784,
    66960,
    67072,
    67120,
    67232,
    67280,
    67344,
    67424,
    67744,
    67856,
    67936,
    68064,
    68160,
    68208,
    68256,
    68320,
    68384,
    68448,
    68528,
    69280,
    69392,
    69520,
    69632,
    69696,
    69808,
    69872,
    69936,
    70048,
    70160
};

SpriteData* sprite_asset(char* pack, u64 handle) {
    return (SpriteData*)&pack[sprite_data_offsets[handle]];
}

u64 primitive_2d_data_offsets[PRIMITIVE_2D_COUNT] = {
    70784
};

Primitive2dData* primitive_2d_asset(char* pack, u64 handle) {
    return (Primitive2dData*)&pack[primitive_2d_data_offsets[handle]];
}

u64 world_data_offsets[WORLD_COUNT] = {
    70848
};

World* world_asset(char* pack, u64 handle) {
    return (World*)&pack[world_data_offsets[handle]];
}

