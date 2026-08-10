typedef struct {
    DrawList       list;
    GlProgram      sprite_program;
    GlProgram      screen_program;
    GlFramebuffer  framebuffer;
    GlUbo          sprite_ubo;
    GlVertexObject quad_mesh;
    GlTexture      palette_texture;
    GlTexture      atlas_texture;
} RendererGL;

void gl_render_init(RendererGL * renderer, char* assets) {
	gl_init_start();

	renderer->sprite_program = gl_create_program_from_files("shaders/sprite.vert", "shaders/sprite.frag");
	glUseProgram(renderer->sprite_program.id);
	glUniform1i(glGetUniformLocation(renderer->sprite_program.id, "palette"), 0);
    glUniform1i(glGetUniformLocation(renderer->sprite_program.id, "atlas"), 1);
	renderer->screen_program = gl_create_program_from_files("shaders/screen.vert", "shaders/screen.frag");
    renderer->framebuffer = gl_create_framebuffer(iv2_new(64, 64));
	renderer->sprite_ubo = gl_create_ubo(DRAW_MAX_SPRITES * sizeof(DrawSprite), 0);

	u32 quad_attrib_size = 2;
	Primitive2dData* quad = primitive_2d_asset(assets, PRIMITIVE_2D_QUAD);
	renderer->quad_mesh = gl_create_vertex_object(&quad_attrib_size, 1, quad->vertices_len, (f32*)quad->vertices);

	TextureData* atlas = texture_asset(assets, TEXTURE_SPRITE_ATLAS);
	renderer->atlas_texture = gl_create_texture(
    	atlas->width, atlas->height, 
    	texture_format_bytes_per_pixel(atlas->format), atlas->pixel_buffer,
    	GL_MIRRORED_REPEAT, GL_NEAREST, GL_NEAREST, 
        false);

	TextureData* palette = texture_asset(assets, TEXTURE_PALETTE);
	renderer->palette_texture = gl_create_texture(
    	palette->width, palette->height, 
    	texture_format_bytes_per_pixel(palette->format), palette->pixel_buffer,
    	GL_MIRRORED_REPEAT, GL_NEAREST, GL_NEAREST, 
        false);
	
	gl_init_end();
}

void gl_render_update(RendererGL* renderer, char* assets) {
    DrawList* list = &renderer->list;
	glViewport(0, 0, list->window_size.x, list->window_size.y);
    gl_clear_color(v4_zero());

    // Render to pixel framebuffer
    //void* p_sprite_ubo = glMapBufferRange(GL_UNIFORM_BUFFER, 0, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    //assert(p_sprite_ubo != NULL);
	//memcpy(p_sprite_ubo, list->sprites, list->sprites_len * sizeof(DrawSprite));
	//glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindFramebuffer(GL_FRAMEBUFFER, renderer->framebuffer.id);
    glViewport(0, 0, renderer->framebuffer.size.x, renderer->framebuffer.size.y);
    gl_clear_color(list->clear_color);
	glUseProgram(renderer->sprite_program.id);
	glBindBufferBase(GL_UNIFORM_BUFFER, renderer->sprite_ubo.binding, renderer->sprite_ubo.id);
	glBindVertexArray(renderer->quad_mesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->palette_texture.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderer->atlas_texture.id);
    Primitive2dData* quad = primitive_2d_asset(assets, PRIMITIVE_2D_QUAD);

	glBindBuffer(GL_UNIFORM_BUFFER, renderer->sprite_ubo.id);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, list->sprites_len * sizeof(DrawSprite), list->sprites); 
	glDrawArraysInstanced(GL_TRIANGLES, 0, quad->vertices_len, list->sprites_len);

	glBindBuffer(GL_UNIFORM_BUFFER, renderer->sprite_ubo.id);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, list->sprites_above_len * sizeof(DrawSprite), list->sprites_above); 
	glDrawArraysInstanced(GL_TRIANGLES, 0, quad->vertices_len, list->sprites_above_len);

    // Render framebuffer on screen quad
    i32 viewport_length;
    i32 viewport_buffer_x = 0;
    i32 viewport_buffer_y = 0;
    if(list->window_size.x > list->window_size.y) {
        viewport_length = list->window_size.y;
        viewport_buffer_x = (list->window_size.x - list->window_size.y) / 2;
    } else {
        viewport_length = list->window_size.x;
        viewport_buffer_y = (list->window_size.y - list->window_size.x) / 2;
    }
	glViewport(viewport_buffer_x, viewport_buffer_y, viewport_length, viewport_length);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(renderer->screen_program.id);
	glBindVertexArray(renderer->quad_mesh.vao);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderer->framebuffer.texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void gl_render_viewport(RendererGL* renderer, iv2 size) {
    glViewport(0, 0, size.x, size.y);
    //renderer->list.window_size = size;
}
