#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform objectInfo {
    uint textureIndex;
    uint shininess;
    vec3 pos;
    vec2 rotation;
} info;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec3 pass_fragColor;
layout(location = 1) out vec2 pass_fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0) + vec4(info.pos, 0);
    pass_fragColor = fragColor;
    pass_fragTexCoord = fragTexCoord;
}