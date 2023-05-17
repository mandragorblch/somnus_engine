#version 450

layout (location = 0) in vec2 position;

layout (location = 1) in vec2 uv;

layout (location = 0) out vec2 pass_uv;

layout (binding = 0) uniform ui_info {
	mat4 ortho;
} info;

void main() {
	gl_Position = vec4(position, 0, 1.f);

	pass_uv = uv;
}