#version 430 core
precision highp float;
layout (location = 0) in vec2 vert; // -1.0 -> 1.0

struct Sprite {
	vec4 src;
	vec4 dst; // screen edges = 0.0 -> 1.0
	vec4 palette_offset; // vec4 for padding
};

layout(std140) uniform sprite_ubo
{
	Sprite sprites[128];
};

out vec2 uv;
out float palette_offset;

void main()
{
	Sprite sprite = sprites[gl_InstanceID];

	// Vert position
	vec2 normal_vert = vert / vec2(2.0f) + vec2(0.5f);
	vec2 pos2d = sprite.dst.xy + sprite.dst.zw * normal_vert;
	gl_Position = vec4(pos2d, 0.0f, 1.0f);

	// UV coordinates
	//vec2 flipped_vert = vec2(normal_vert.x, 1.0f - normal_vert.y);
	uv = sprite.src.xy + sprite.src.zw * normal_vert;

	// Text color
	palette_offset = sprite.palette_offset.x;

    // DEBUG
    //uv = normal_vert;
	//gl_Position = vec4(vert, 0.0f, 1.0f);;
}
