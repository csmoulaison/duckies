#version 430 core
precision mediump float;
in vec2 uv;
in float palette_offset;
out vec4 frag_color;

uniform sampler2D palette;
uniform sampler2D atlas;

void main()
{
    float index = texture(atlas, uv).r * 1.0f;
    frag_color = texture(palette, vec2(index + (palette_offset / 256.0f), 0.0f));
}
