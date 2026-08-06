#version 430 core
precision highp float;
layout (location = 0) in vec2 vert;

out vec2 uv;

void main() {
	vec2 normal_vert = vert / vec2(2.0f) + vec2(0.5f);
    gl_Position = vec4(vert.x, vert.y, 0.0, 1.0); 
    uv = normal_vert;
}
