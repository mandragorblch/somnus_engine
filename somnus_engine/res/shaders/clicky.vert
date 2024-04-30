#version 460

#extension GL_ARB_shader_draw_parameters : enable

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in ivec4 boneIds; 
layout(location = 5) in vec4 weights;
//------------------------------------------------------------------------------------------------------------------------------
struct physicalObjectInfo {
    vec4 pos_index;
    vec4 quat_rot;
    vec4 velocity_mass;
    vec4 angularVelocity_momentOfInertia;
    vec4 force;
    uint type;
    float lifeTime;
    bool should_be_deleted;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 projView;
} ubo;

layout(std430, binding = 1) buffer physicalObjectInfoIn {
	physicalObjectInfo physicalObjectsInfo[];
};

layout(binding = 2) buffer bonesMatrices {
    mat4 matrices[ ];
} finalBonesMatrices;//WRITE OBJECT INFO BUFFER(INDEX)

layout(binding = 3) buffer objectInfo {
    uint index[16384];
} info;

vec3 multiply_quats_to_vec3(const vec4 first, const vec4 second) {
	return vec3(first.w * second.xyz + second.w * first.xyz + vec3(first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x));
}

vec4 multiply_quat_vec3(const vec4 quat, const vec3 vec) {
	return vec4(quat.w * vec + vec3(quat.y * vec.z - quat.z * vec.y, quat.z * vec.x - quat.x * vec.z, quat.x * vec.y - quat.y * vec.x), -(quat.x * vec.x + quat.y * vec.y + quat.z * vec.z));
}

vec3 rotation_by_quat(const vec3 pos, const vec4 quat) {
	return multiply_quats_to_vec3(multiply_quat_vec3(quat, pos), vec4(-quat.xyz, quat.w));
}

//out gl_PerVertex {
//    vec4 gl_Position;   
//};

layout(location = 0) out flat uint PO_index;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
const float deltaTime_144Hz = 1./144.;
const float deltaTime_60Hz = 1./60.;
 
void main()
{
    float currentDeltaTime = deltaTime_60Hz;
    mat4 weighted_skeleton_transform_matrix = mat4(0);
    PO_index = gl_BaseInstanceARB;

    physicalObjectInfo current_physicalObjectInfo = physicalObjectsInfo[PO_index];
    vec3 finalPosition = pos;
    bool is_aminated = false;

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            break;
        }
        weighted_skeleton_transform_matrix += finalBonesMatrices.matrices[boneIds[i]] * weights[i];
        is_aminated = true;
    }

    if(is_aminated) {
        vec4 pos_scaled = weighted_skeleton_transform_matrix * vec4(pos, 1.);
        finalPosition = pos_scaled.xyz / pos_scaled.w;
    }

//    if(current_physicalObjectInfo.type == 1){
        finalPosition = rotation_by_quat(finalPosition, current_physicalObjectInfo.quat_rot) + current_physicalObjectInfo.pos_index.xyz;
//	} else {
//        if(current_physicalObjectInfo.should_be_deleted){
//            finalPosition = vec3(0);
//        } else {
//            finalPosition = finalPosition * min(current_physicalObjectInfo.lifeTime, 1.0) + current_physicalObjectInfo.pos_index.xyz;
//        }
//    }

    gl_Position =  ubo.projView * vec4(finalPosition, 1.0);
}