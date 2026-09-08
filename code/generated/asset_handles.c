// Pregenerated file. Any changes made will be erased on recompilation.

#define TEXTURE_COUNT 2
#define TEXTURE_SPRITE_ATLAS 0
#define TEXTURE_PALETTE 1

#define SPRITE_COUNT 89
#define SPRITE_CAR_B 0
#define SPRITE_TREE 1
#define SPRITE_HANNAH_TEA 2
#define SPRITE_DUCK_EXPLODE 3
#define SPRITE_SNAKE_BODY 4
#define SPRITE_FROG 5
#define SPRITE_FONT_SMALL 6
#define SPRITE_LILY 7
#define SPRITE_BRIDGE 8
#define SPRITE_FROGO_PORTRAIT 9
#define SPRITE_BUTTON 10
#define SPRITE_TRUCK_RIGHT 11
#define SPRITE_FROG_LEAP_RIGHT 12
#define SPRITE_FROG_TEA 13
#define SPRITE_QUESTION 14
#define SPRITE_HANNAH_LEFT 15
#define SPRITE_NUMS 16
#define SPRITE_DUCK_BONK_LEFT 17
#define SPRITE_DUCK_JUMP 18
#define SPRITE_DUCK_BONK_RIGHT 19
#define SPRITE_DUCK_PORTRAIT 20
#define SPRITE_TROLL_TEA 21
#define SPRITE_TROLL_PORTRAIT 22
#define SPRITE_TROLL 23
#define SPRITE_DUCK_BONK_DOWN 24
#define SPRITE_DUCK_LEFT 25
#define SPRITE_FROG_LEAP_DOWN 26
#define SPRITE_FROG_LEAP_UP 27
#define SPRITE_SNAKE_PORTRAIT 28
#define SPRITE_HANNAH_WAIT 29
#define SPRITE_DEBUG_CIRCLE 30
#define SPRITE_SPRITE_MAN_PORTRAIT 31
#define SPRITE_HANNAH_BONK_RIGHT 32
#define SPRITE_CLIFF 33
#define SPRITE_WELL 34
#define SPRITE_ZEEZ 35
#define SPRITE_TROLL_DANCE 36
#define SPRITE_MAP_A 37
#define SPRITE_FROG_LEAP_LEFT 38
#define SPRITE_SHRUB 39
#define SPRITE_PUFF 40
#define SPRITE_DUCK_TEA 41
#define SPRITE_HAPPY_MASK 42
#define SPRITE_SNAKE_TAIL 43
#define SPRITE_HANNAH_BONK_DOWN 44
#define SPRITE_CRUMBLER 45
#define SPRITE_SNAKE_TEA 46
#define SPRITE_HANNAH_DROWN 47
#define SPRITE_GATE 48
#define SPRITE_HANNAH_BONK_LEFT 49
#define SPRITE_HANNAH_BONK_UP 50
#define SPRITE_TRUCK 51
#define SPRITE_DUCK_DOWN 52
#define SPRITE_DUCK_THUMBS_UP 53
#define SPRITE_SIGN 54
#define SPRITE_FROG_PORTRAIT 55
#define SPRITE_DUCK_UP 56
#define SPRITE_CAR_A 57
#define SPRITE_LILY_SINK 58
#define SPRITE_HANNAH_PORTRAIT 59
#define SPRITE_LARGE_GATE 60
#define SPRITE_DIALOGUE_BOX 61
#define SPRITE_WASD 62
#define SPRITE_ROAD 63
#define SPRITE_MUFFIN_PORTRAIT 64
#define SPRITE_EXCLAMATION 65
#define SPRITE_MUFFIN_LEFT 66
#define SPRITE_FONT_BIG 67
#define SPRITE_MAN 68
#define SPRITE_BUS 69
#define SPRITE_HANNAH_FRONT 70
#define SPRITE_TRUCK_LEFT 71
#define SPRITE_MAP_SMALL 72
#define SPRITE_HANNAH_UP 73
#define SPRITE_WATER 74
#define SPRITE_HANNAH_SIT 75
#define SPRITE_DUCK_RIGHT 76
#define SPRITE_GRASS 77
#define SPRITE_EGG_RUMBLE 78
#define SPRITE_CURSOR 79
#define SPRITE_HANNAH_DOWN 80
#define SPRITE_SNAKE_HEAD 81
#define SPRITE_TROLL_TALK 82
#define SPRITE_HANNAH_RIGHT 83
#define SPRITE_MAN_PORTRAIT 84
#define SPRITE_MUFFIN_TEA 85
#define SPRITE_DUCK_BONK_UP 86
#define SPRITE_MUFFIN 87
#define SPRITE_APPLE 88

#define PRIMITIVE_2D_COUNT 1
#define PRIMITIVE_2D_QUAD 0

#define WORLD_COUNT 1
#define WORLD_MAIN 0

u64 texture_data_offsets[TEXTURE_COUNT] = {
    0,
    271808
};

TextureData* texture_asset(char* pack, u64 handle) {
    return (TextureData*)&pack[texture_data_offsets[handle]];
}

u64 sprite_data_offsets[SPRITE_COUNT] = {
    262176,
    262256,
    262304,
    262368,
    262496,
    262752,
    262816,
    263568,
    263648,
    263712,
    263760,
    263840,
    263920,
    263984,
    264048,
    264096,
    264208,
    264384,
    264464,
    264576,
    264656,
    264704,
    264768,
    264816,
    264880,
    265056,
    265168,
    265232,
    265296,
    265344,
    265392,
    265456,
    265536,
    265616,
    265936,
    265984,
    266048,
    266160,
    266208,
    266272,
    266320,
    266432,
    266496,
    266576,
    266752,
    266832,
    266912,
    266976,
    267040,
    267168,
    267248,
    267328,
    267408,
    267520,
    267616,
    267664,
    267712,
    267824,
    267904,
    267968,
    268080,
    268304,
    268368,
    268544,
    268624,
    268672,
    268720,
    268768,
    269520,
    269632,
    269712,
    269840,
    269920,
    269968,
    270080,
    270144,
    270208,
    270320,
    270384,
    270800,
    270864,
    270976,
    271152,
    271216,
    271328,
    271408,
    271472,
    271552,
    271760
};

SpriteData* sprite_asset(char* pack, u64 handle) {
    return (SpriteData*)&pack[sprite_data_offsets[handle]];
}

u64 primitive_2d_data_offsets[PRIMITIVE_2D_COUNT] = {
    272864
};

Primitive2dData* primitive_2d_asset(char* pack, u64 handle) {
    return (Primitive2dData*)&pack[primitive_2d_data_offsets[handle]];
}

u64 world_data_offsets[WORLD_COUNT] = {
    272928
};

World* world_asset(char* pack, u64 handle) {
    return (World*)&pack[world_data_offsets[handle]];
}

