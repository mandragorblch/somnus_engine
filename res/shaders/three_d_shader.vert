#version 460

#extension GL_ARB_shader_draw_parameters : enable
//#extension SPV_KHR_shader_draw_parameters : enable
//--------------------------------------------------------------------------------------/
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

//struct lightInfo {
//	mat4 model;
//    mat4 view;
//    mat4 proj;
//    vec3 lightPos;
//    vec3 lightDir;
//};

layout(std430, binding = 0) readonly buffer physicalObjectInfoIn {
	physicalObjectInfo physicalObjectsInfo[];
};

layout(binding = 1) uniform UniformBufferObject {
    mat4 projView;
} ubo;

layout(binding = 2) buffer objectInfo {
    uint index[16384];
} info;

layout(binding = 3) buffer bonesMatrices {
    mat4 matrices[ ];
} finalBonesMatrices;

//layout(binding = 4) uniform LightMatrix {
//    lightInfo lightInfos[2];
////    float sin_of_angle;
//};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in ivec4 boneIds; 
layout(location = 5) in vec4 weights;

layout(location = 0) out vec3 pass_fragColor;
layout(location = 1) out vec2 pass_fragTexCoord;
layout(location = 2) out vec3 pass_normal;
layout(location = 3) out vec3 pass_position;
layout(location = 4) out vec3 pass_velocity;
//layout(location = 5) out vec3 pass_lightDir;
//layout(location = 6) out vec3 pass_TBN_lightDir;
//layout(location = 7) out float pass_theta;
//layout(location = 8) out float pass_milleage;
layout(location = 5) out mat3 TBN;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
const float deltaTime_144Hz = 1./144.;
const float deltaTime_60Hz = 1./60.;

//const mat4 biasMat = mat4( 
//	0.5, 0.0, 0.0, 0.0,
//	0.0, 0.5, 0.0, 0.0,
//	0.0, 0.0, 1.0, 0.0,
//	0.5, 0.5, 0.0, 1.0 );

vec3 multiply_quats_to_vec3(const vec4 first, const vec4 second) {
	return vec3(first.w * second.xyz + second.w * first.xyz + vec3(first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x));
}

vec4 multiply_quat_vec3(const vec4 quat, const vec3 vec) {
	return vec4(quat.w * vec + vec3(quat.y * vec.z - quat.z * vec.y, quat.z * vec.x - quat.x * vec.z, quat.x * vec.y - quat.y * vec.x), -(quat.x * vec.x + quat.y * vec.y + quat.z * vec.z));
}

vec3 rotation_by_quat(const vec3 pos, const vec4 quat) {
	return multiply_quats_to_vec3(multiply_quat_vec3(quat, pos), vec4(-quat.xyz, quat.w));
}

void main() {
    float currentDeltaTime = deltaTime_60Hz;
    uint index = gl_BaseInstanceARB;

    physicalObjectInfo current_physicalObjectInfo = physicalObjectsInfo[index];

    vec3 finalPosition = pos;
    mat4 weighted_skeleton_transform_matrix = mat4(0);
    vec3 totalNormal = normal;
    vec3 totalTangent = tangent;

    bool is_animated = false;
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            break;
        }
        weighted_skeleton_transform_matrix += finalBonesMatrices.matrices[boneIds[i]] * weights[i];
        is_animated = true;
    }

    if(is_animated) {
        vec4 pos_scaled = weighted_skeleton_transform_matrix * vec4(finalPosition, 1.);
        vec4 normal_scaled = transpose(inverse(weighted_skeleton_transform_matrix)) * vec4(totalNormal, 1);
        vec4 tangent_scaled = transpose(inverse(weighted_skeleton_transform_matrix)) * vec4(totalTangent, 1);

        finalPosition = pos_scaled.xyz / pos_scaled.w;
        totalNormal   = normal_scaled.xyz;
        totalTangent  = tangent_scaled.xyz;
    }

    totalNormal = rotation_by_quat(totalNormal, current_physicalObjectInfo.quat_rot);

//    if(current_physicalObjectInfo.lifeTime > 0.0 && !current_physicalObjectInfo.should_be_deleted){
        finalPosition = rotation_by_quat(finalPosition, current_physicalObjectInfo.quat_rot) + current_physicalObjectInfo.pos_index.xyz;
//	} else {
//        finalPosition = vec3(0);
//    }

    gl_Position = ubo.projView * vec4(finalPosition, 1.0);
//    gl_Position.w += current_physicalObjectInfo.lifeTime*10;
//    if(tangent.x == 0 && tangent.y == 0 && tangent.z == 0) {
//        pass_fragColor = totalTangent;
//    }

    if (tangent != vec3(0.0)) {
        vec3 T = normalize(totalTangent);
        vec3 N = normalize(totalNormal);
        vec3 B = normalize(cross(N, T));
        T = normalize(T - N * dot(N, T));
        TBN = mat3(T, B, N);
    } else {
        TBN = mat3(0.0);
    }

    pass_fragTexCoord = fragTexCoord;

    pass_normal = totalNormal;

    pass_position = finalPosition;
//    pass_velocity = vec3(index == 0);

//    pass_position_lightSpace = (biasMat * lightInfos[0].proj * lightInfos[0].view) * vec4(finalPosition, 1.0);
//
//    pass_lightDir = normalize(lightInfos[0].lightPos - finalPosition);
//
//    pass_TBN_lightDir = TBN * pass_lightDir;
//
//    pass_theta = dot(pass_lightDir, -lightInfos[0].lightDir);
//
//    pass_milleage = length(lightInfos[0].lightPos - finalPosition) + 5.0;
}
