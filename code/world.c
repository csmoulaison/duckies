#define LEVELS_MAX 1024

#define WORLD_PATH_FROM_BIN    "../assets/world/world.data"
#define WORLD_PATH_FROM_BUILD "assets/world/world.data"

typedef enum {
    TILE_TYPE_GROUND,
    TILE_TYPE_WATER,
    TILE_TYPE_COUNT 
} TileType;

typedef struct {
    u16 type;
    u16 flags;
} Tile;

typedef struct {
    u16  exit_up;
    u16  exit_left;
    u16  exit_down;
    u16  exit_right;
    Tile tiles[64];
} Level;

typedef struct {
    Level levels[LEVELS_MAX];
} World;

World* world_from_file(File* file, Stack* stack) {
    World* world = (World*)stack_alloc(stack, sizeof(World));
    file_read(file, world, sizeof(World));
    return world;
}

void world_save(File* file, World* world) {
    file_write(file, world, sizeof(World));
}

i32 index_from_pos(iv2 pos) {
    return pos.y * 8 + pos.x;
}

iv2 pos_from_index(i32 index) {
    return iv2_new(index % 8, index / 8);
}

v2 pixel_pos_from_index(i32 index) {
    return v2_scale(v2_new(index % 8, index / 8), 8.0f);
}

Tile tile_from_pos(Level* level, iv2 pos) {
    return level->tiles[index_from_pos(pos)];
}
