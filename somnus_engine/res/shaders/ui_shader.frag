#version 460

layout (binding = 1) uniform sampler2D tex0;

layout (location = 0) in vec2 pass_uv;

layout (location = 1) in flat int is_interacted;

layout(location = 0) out vec4 color;

void main() {
	vec4 texture_color = texture(tex0, pass_uv);
	vec3 gamma = vec3(2.2);
	if(is_interacted == 1) {
		color = texture_color + vec4(vec3(0.3), 0.0);
	} else {
		color = vec4(texture_color.rgb, texture_color.w);
	}
}