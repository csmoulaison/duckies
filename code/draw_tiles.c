#define ADJ_U   1 << 0
#define ADJ_L   1 << 1
#define ADJ_D   1 << 2
#define ADJ_R   1 << 3
#define ADJ_UL  1 << 4
#define ADJ_UR  1 << 5
#define ADJ_DL  1 << 6
#define ADJ_DR  1 << 7

bool offset_tile_is_cliff(iv2 root, iv2 offset, LevelPacket* packet) {
    iv2 pos = iv2_add(root, offset);
    Tile tile = tile_from_pos_and_level_packet(pos, packet);
    if(tile.flags & TILE_FLAG_CLIFF) {
        return true;
    }
    return false;
}

bool tile_up   (iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(0, 1),   levels); }
bool tile_left (iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(-1, 0),  levels); }
bool tile_down (iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(0, -1),  levels); }
bool tile_right(iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(1, 0),   levels); }
bool tile_ul   (iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(-1, 1),  levels); }
bool tile_ur   (iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(1, 1),   levels); }
bool tile_dl   (iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(-1, -1), levels); }
bool tile_dr   (iv2 pos, LevelPacket* levels) { return offset_tile_is_cliff(pos, iv2_new(1, -1),  levels); }

#define BIT_MATCH(value, mask) ((value & (mask)) == (mask))

i32 tile_cliff_frame(iv2 p, LevelPacket* pk, i32* out_cap) {
    u8 adj = 0;
    if(tile_up(p, pk))    adj = adj | ADJ_U;
    if(tile_left(p, pk))  adj = adj | ADJ_L;
    if(tile_down(p, pk))  adj = adj | ADJ_D;
    if(tile_right(p, pk)) adj = adj | ADJ_R;
    if(tile_ul(p, pk))    adj = adj | ADJ_UL;
    if(tile_ur(p, pk))    adj = adj | ADJ_UR;
    if(tile_dl(p, pk))    adj = adj | ADJ_DL;
    if(tile_dr(p, pk))    adj = adj | ADJ_DR;

    bool u = adj & ADJ_U;
    bool l = adj & ADJ_L;
    bool d = adj & ADJ_D;
    bool r = adj & ADJ_R;
    bool ul = adj & ADJ_UL;
    bool ur = adj & ADJ_UR;
    bool dl = adj & ADJ_DL;
    bool dr = adj & ADJ_DR;

    *out_cap = -1;

/*  u8 corner_potential = ADJ_U | ADJ_D;
    if((adj & corner_potential) == corner_potential) {
        i32 corner_seg = -1;
        if((adj & (ADJ_L | ADJ_R) == (ADJ_L | ADJ_R)) {
            corner_seg = 0;
            if((adj & (ADJ
        }

        i32 corner_off = -1;
        if((adj & ADJ_D) == 0) {
        }

        if(corner_seg != -1 && corner_off != -1) {
            return ...
        }
    } */

    i32 offset = -1;
    i32 segment = -1;

    // Check if we are upper corner.
    if(d) {
        if(l && r) {
            segment = 3;
            if(!(dl || dr)) {
                offset = 3;
            } else if(!dr) {
                offset = 2;
            } else if(!dl) {
                offset = 0;
            } else {
                segment = 1;
                offset = 1;
            }
        } else if(l) {
            if(!dl) {
                segment = 5;
                offset = 2;
            }
        } else if(r) {
            if(!dr) {
                segment = 5;
                offset = 3;
            }
        }
    }

    // And lower corner.
    if(segment == -1 && d) {
        segment = 4;
        if(l && r && !(dr || dl)) {
            offset = 3;
        } else if((!r) && dr) {
            if(!l) {
                segment = 5;
                offset = 1;
            } else {
                offset = 2;
            }
        } else if((!l) && dl) {
            if(!r) {
                segment = 5;
                offset = 0;
            } else {
                offset = 0;
            }
        } else {
            segment = -1;
        }
    }

    if(segment == -1) {
        // Are we at the bottom or the v middle?
        segment = -1;
        if(adj & ADJ_D) {
            segment = 1;
        } else {
            segment = 0;
        }
        assert(segment != -1);

        // Are we at the left, right or h middle?
        if(adj & ADJ_L) {
            if(adj & ADJ_R) {
                offset = 1;
            } else {
                offset = 2;
            }
        } else if(adj & ADJ_R) {
            offset = 0;
        } else {
            offset = 3;
        }
        assert(offset != -1);
    }

    // Are we at the top of the cliff and therefore need a cap?
    if(u) {
        *out_cap = -1;
    } else {
        i32 cap_offset = -1;
        if(l && r) {
            cap_offset = 1;
        } else if(l) {
            cap_offset = 2;
        } else if(r) {
            cap_offset = 0;
        } else {
            cap_offset = 3;
        }
        assert(cap_offset != -1);
        *out_cap = 8 + cap_offset;
    }

    return segment * 4 + offset;
}

