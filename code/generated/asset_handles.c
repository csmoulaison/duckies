// Pregenerated file. Any changes made will be erased on recompilation.

#define TEXTURE_COUNT 2
#define TEXTURE_SPRITE_ATLAS 0
#define TEXTURE_PALETTE 1

#define SPRITE_COUNT 19
#define SPRITE_LILY 0
#define SPRITE_HANNAH_LEFT 1
#define SPRITE_NUMS 2
#define SPRITE_DUCK_JUMP 3
#define SPRITE_DUCK_LEFT 4
#define SPRITE_DEBUG_CIRCLE 5
#define SPRITE_HAPPY_MASK 6
#define SPRITE_DUCK_THUMBS_UP 7
#define SPRITE_LILY_SINK 8
#define SPRITE_FONT_BIG 9
#define SPRITE_MAN 10
#define SPRITE_HANNAH_FRONT 11
#define SPRITE_HANNAH_UP 12
#define SPRITE_WATER 13
#define SPRITE_DUCK_RIGHT 14
#define SPRITE_GRASS 15
#define SPRITE_CURSOR 16
#define SPRITE_HANNAH_DOWN 17
#define SPRITE_HANNAH_RIGHT 18

#define PRIMITIVE_2D_COUNT 1
#define PRIMITIVE_2D_QUAD 0

#define WORLD_COUNT 1
#define WORLD_MAIN 0

u64 texture_data_offsets[TEXTURE_COUNT] = {
    0,
    68096
};

TextureData* texture_asset(char* pack, u64 handle) {
    return (TextureData*)&pack[texture_data_offsets[handle]];
}

u64 sprite_data_offsets[SPRITE_COUNT] = {
    65568,
    65648,
    65760,
    65936,
    66048,
    66160,
    66224,
    66304,
    66400,
    66464,
    67216,
    67328,
    67456,
    67568,
    67632,
    67744,
    67808,
    67872,
    67984
};

SpriteData* sprite_asset(char* pack, u64 handle) {
    return (SpriteData*)&pack[sprite_data_offsets[handle]];
}

u64 primitive_2d_data_offsets[PRIMITIVE_2D_COUNT] = {
    68384
};

Primitive2dData* primitive_2d_asset(char* pack, u64 handle) {
    return (Primitive2dData*)&pack[primitive_2d_data_offsets[handle]];
}

u64 world_data_offsets[WORLD_COUNT] = {
    68448
};

World* world_asset(char* pack, u64 handle) {
    return (World*)&pack[world_data_offsets[handle]];
}

