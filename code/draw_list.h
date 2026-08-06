#ifndef draw_list_h_INCLUDED
#define draw_list_h_INCLUDED

#define DRAW_MAX_SPRITES      128
#define LOGICAL_WINDOW_WIDTH  64
#define LOGICAL_WINDOW_HEIGHT 64

typedef struct {
    v4 src;
    v4 dst;
    f32 palette_offset;
    f32 pad[3];
} DrawSprite;

typedef struct {
    v2         window_size;
    v2         logical_size;
    i32        pixel_scale;

    v4         clear_color;
    DrawSprite sprites[DRAW_MAX_SPRITES];
    u32        sprites_len;

    // cheeky state
    bool       minified;
} DrawList;

void draw_init_list(DrawList* list, iv2 window_size) {
    memset(list, 0, sizeof(DrawList));
    list->window_size = v2_new(window_size.x, window_size.y);
    list->logical_size = v2_new(64, 64);
}

#endif // draw_list_h_INCLUDED
