#version 430 core
precision mediump float;
layout (location = 0) in vec2 vert;

out vec2 uv;

void main() {
    uv = vert;
}
