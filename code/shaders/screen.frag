#version 430 core
precision mediump float;
in vec2 uv;
out vec4 frag_color;

uniform sampler2D screen;

void main() {
    frag_color = vec4(texture(screen, uv).rgb, 1.0f);
}
