#version 450
out vec4 color;

layout (location = 0) in vec2 pass_uv;

layout (binding = 1) uniform sampler2D tex0;

void main() {
//    color = vec4(1);
	color = texture(tex0, pass_uv);
}