#version 460

layout (binding = 0) uniform ui_info {
	vec2 pos;
	vec3 rot;
	bool is_interacted;
	uint texture_index;
} info;

layout (location = 0) in vec2 position;

layout (location = 1) in vec2 uv;

layout (location = 0) out vec2 pass_uv;

layout (location = 1) out flat int pass_is_interacted;

vec3 rotate_x(vec3 pos){
	return (pos * mat3(1, 0,				   0,
					   0, cos(info.rot[0]), -sin(info.rot[0]),
					   0, sin(info.rot[0]),  cos(info.rot[0])));
}

vec3 rotate_y(vec3 pos){
	return (pos * mat3(cos(info.rot[1]), 0, sin(info.rot[1]),
					   0,				 1, 0,
					  -sin(info.rot[1]), 0, cos(info.rot[1])));
}

vec3 rotate_z(vec3 pos){
	return (pos * mat3(cos(info.rot[2]), -sin(info.rot[2]), 0,
					   sin(info.rot[2]),  cos(info.rot[2]), 0,
		   			   0,				  0,				1));
}

void main() {
	gl_Position = vec4(position + info.pos, 0, 1.0);

	if (info.is_interacted == true) {
		pass_is_interacted = 1;
	} else {
		pass_is_interacted = 0;
	}
	pass_uv = uv;
}