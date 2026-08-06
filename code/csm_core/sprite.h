#ifndef csm_sprite_h_INCLUDED
#define csm_sprite_h_INCLUDED

typedef struct {
    iv2 atlas_position;
    f32 duration;
} SpriteFrame;

typedef struct {
    u64         texture_handle;
    iv2         origin;
    iv2         size;
    u32         frames_len;
    SpriteFrame frames[];
} SpriteData;

u64 sprite_size_from_frame_count(u64 frame_count);
u64 sprite_size(SpriteData* sprite);

#ifdef CSM_IMPLEMENTATION

u64 sprite_size_from_frame_count(u64 frame_count) {
    return sizeof(SpriteData) + frame_count * sizeof(SpriteFrame);
}

u64 sprite_size(SpriteData* sprite) {
    return sprite_size_from_frame_count(sprite->frames_len);
}

#endif
#endif
