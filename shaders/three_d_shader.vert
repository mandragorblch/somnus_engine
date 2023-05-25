#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform objectInfo {
    uint shininess;
    vec3 pos;
    vec3 rot;
} info;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec3 pass_fragColor;
layout(location = 1) out vec2 pass_fragTexCoord;


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
	vec3 position_after_rotation = rotate_x(inPosition);
//	position_after_rotation = rotate_y(position_after_rotation);
//	position_after_rotation = rotate_z(position_after_rotation);
    gl_Position = ubo.proj * ubo.view * vec4(position_after_rotation, 1.0) + vec4(info.pos, 0);
    pass_fragColor = fragColor;
    pass_fragTexCoord = fragTexCoord;
}