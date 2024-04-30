#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <physx/PxPhysicsAPI.h>

inline glm::vec2 PxVec2_to_vec2(const physx::PxVec2& vector) {
	return glm::vec2(vector.x, vector.y);
}

inline glm::vec3 PxVec3_to_vec3(const physx::PxVec3& vector) {
	return glm::vec3(vector.x, vector.y, vector.z);
}

inline glm::vec4 PxVec4_to_vec4(const physx::PxVec4& vector) {
	return glm::vec4(vector.x, vector.y, vector.z, vector.w);
}

inline glm::vec4 PxQuat_to_vec4_xyzw(const physx::PxQuat& quaternion) {
	return glm::vec4(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

inline glm::quat PxQuat_to_quat(const physx::PxQuat& quaternion)
{
	return glm::quat(quaternion.w, glm::vec3(quaternion.x, quaternion.y, quaternion.z));
}

inline physx::PxVec2 vec2_to_PxVec2(const glm::vec2& vector) {
	return physx::PxVec2(vector.x, vector.y);
}

inline physx::PxVec3 vec3_to_PxVec3(const glm::vec3& vector) {
	return physx::PxVec3(vector.x, vector.y, vector.z);
}

inline physx::PxVec4 vec4_to_PxVec4(const glm::vec4& vector) {
	return physx::PxVec4(vector.x, vector.y, vector.z, vector.w);
}

inline physx::PxQuat quat_to_PxQuat(const glm::quat& vector) {
	return physx::PxQuat(vector.w, vector.x, vector.y, vector.z);
}

inline physx::PxQuat vec4_to_PxQuat(const glm::vec4& vector) {
	return physx::PxQuat(vector.w, vector.x, vector.y, vector.z);
}