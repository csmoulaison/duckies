#include "draw_list.h"

void draw_clear_color(DrawList* list, v4 color) {
    list->clear_color = color;
}

void draw_sprite(DrawList* list, u64 handle, u32 frame_index, v2 position, i32 palette_index) {
	DrawSprite* sprite = &list->sprites[list->sprites_len];
	list->sprites_len++;

    TextureData* texture   = texture_asset(asset_pack_data, TEXTURE_SPRITE_ATLAS);
    SpriteData* asset      = sprite_asset(asset_pack_data, handle);
    SpriteFrame* frame     = &asset->frames[frame_index];

    sprite->src = v4_new(
        (f32)frame->atlas_position.x / texture->width,
        (f32)frame->atlas_position.y / texture->height,
        (f32)asset->size.x           / texture->width,
        (f32)asset->size.y           / texture->height);

    v2 scaled_logical = list->logical_size;
    if(list->minified) {
        scaled_logical = v2_scale(list->logical_size, 2.0f);
        position.x += 32;
        position.y += 32;
    }

    sprite->dst = v4_new(
        position.x    / scaled_logical.x, 
        position.y    / scaled_logical.y,
        asset->size.x / scaled_logical.x, 
        asset->size.y / scaled_logical.y);
    sprite->dst.x = sprite->dst.x * 2.0 - 1.0f;
    sprite->dst.y = sprite->dst.y * 2.0 - 1.0f;
    sprite->dst.z = sprite->dst.z * 2.0;
    sprite->dst.w = sprite->dst.w * 2.0;
    sprite->palette_offset = palette_index * 16.0f;
}

void draw_sprite_animated(DrawList* list, u64 handle, f32 t, v2 position, i32 palette_index) {
    SpriteData* asset = sprite_asset(asset_pack_data, handle);
    i32 frame = (i32)(t * asset->frames_len) % asset->frames_len;
    draw_sprite(list, handle, frame, position, palette_index);
}
