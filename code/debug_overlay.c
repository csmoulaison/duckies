String init_dbg_overlay(char* buf) {
    String dbg_str = string_init(buf, 256);
    assert(dbg_str.capacity == 256);
    return dbg_str;
}

void dbg_println(String* dbg_str, String s) {
    assert(dbg_str->capacity == 256);
    string_cat(dbg_str, s);
    string_cat(dbg_str, string_const("\n"));
}

void dbg_clear(String* dbg_str) {
    dbg_str->len = 0;
}

void dbg_draw(String* dbg_str, DrawList* draw_list) {
    char line_buf[64];
    String line = string_init(line_buf, 64);
    i32 lines = 0;
    for(i32 i = 0; i < dbg_str->len; i++) {
        if(dbg_str->text[i] == '\n') {
            draw_simple_text(draw_list, line, v2_new(0, 55 - lines * 5), 0);
            lines++;
            line.len = 0;
        } else {
            string_write_char(&line, dbg_str->text[i]);
        }
    }

}
