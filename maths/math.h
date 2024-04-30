#pragma once
#ifndef CUSTOM_MATH
#define CUSTOM_MATH

#include "glm/glm.hpp"
#include <bit>
#include <limits>
#include <cstdint>
#include "utility/better_debugging.h"

namespace custom_maths {
	inline float abs(float val) {
		reinterpret_cast<int&>(val) &= 0b01111111111111111111111111111111;
		return val;
	}

	inline void neg(float& val) {
		reinterpret_cast<int&>(val) ^= 0b10000000000000000000000000000000;
	}

	constexpr inline float Q_rsqrt(float number) noexcept
	{
		static_assert(std::numeric_limits<float>::is_iec559); // (enable only on IEEE 754)

		const float y = std::bit_cast<float>(
			0x5f3759df - (std::bit_cast<std::uint32_t>(number) >> 1));
		return y * (1.5f - (number * 0.5f * y * y));
	}

	constexpr inline glm::vec3 cross(const glm::vec3& first, const glm::vec3& second) {
		return glm::vec3(first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x);
	}

	constexpr inline float dot(const glm::vec3& first, const glm::vec3& second) {
		return first.x * second.x + first.y * second.y + first.z * second.z;
	}

	constexpr inline glm::vec3 fast_normalize_glm_vec3(const glm::vec3& vec) {
		return (vec * Q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
	}

	constexpr inline glm::vec4 fast_normalize_glm_vec4(const glm::vec4& vec) {
		return (vec * Q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w));
	}

	constexpr inline glm::vec4 fast_normalize_glm_vec4(glm::vec4&& vec) {
		return (vec * Q_rsqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w));
	}

	constexpr inline glm::vec3 normalize_glm_vec3(const glm::vec3& vec) {
		return (vec / sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
	}

	constexpr inline glm::vec4 normalize_glm_vec4(const glm::vec4& vec) {
		return (vec / sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w));
	}

	inline glm::mat3 rotateX_matrix(float angle)
	{
		float sinf = std::sinf(angle);
		float cosf = std::cosf(angle);
		return glm::mat3(1, 0, 0,
			0, cosf, -sinf,
			0, sinf, cosf);
	}

	inline glm::mat3 rotateY_matrix(float angle)
	{
		float sinf = std::sinf(angle);
		float cosf = std::cosf(angle);
		return glm::mat3(cosf, 0, sinf,
			0, 1, 0,
			-sinf, 0, cosf);
	}

	inline glm::mat3 rotateZ_matrix(float angle)
	{
		float sinf = std::sinf(angle);
		float cosf = std::cosf(angle);
		return glm::mat3(cosf, sinf, 0,
			sinf, cosf, 0,
			0, 0, 1);
	}

	inline glm::vec3 rotate(const glm::vec3& axis, const glm::vec3& pos, float angle) {
		float sinf = std::sinf(angle);
		float cosf = std::cosf(angle);
		float cosf_minus_one = (1 - cosf);
		return  (pos * glm::mat3(cosf + cosf_minus_one * axis.x * axis.x, cosf_minus_one * axis.x * axis.y - sinf * axis.z, cosf_minus_one * axis.x * axis.z + sinf * axis.y,
			cosf_minus_one * axis.y * axis.x + sinf * axis.z, cosf + cosf_minus_one * axis.y * axis.y, cosf_minus_one * axis.y * axis.z - sinf * axis.x,
			cosf_minus_one * axis.z * axis.x - sinf * axis.y, cosf_minus_one * axis.z * axis.y + sinf * axis.x, cosf + cosf_minus_one * axis.z * axis.z));
	}

	constexpr inline glm::vec4 multiply_quats(const glm::vec4& first, const glm::vec4& second) {
		return glm::vec4(first.w * glm::vec3(second) + second.w * glm::vec3(first) + glm::vec3(first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x), (first.w * second.w - (first.x * second.x + first.y * second.y + first.z * second.z)));
	}

	constexpr inline glm::vec3 multiply_quats_to_vec3(const glm::vec4& first, const glm::vec4& second) {
		return glm::vec3(first.w * glm::vec3(second) + second.w * glm::vec3(first) + glm::vec3(first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x));
	}

	constexpr inline glm::vec4 multiply_quat_vec3(const glm::vec4& quat, const glm::vec3& vec) {
		return glm::vec4(quat.w * vec + glm::vec3(quat.y * vec.z - quat.z * vec.y, quat.z * vec.x - quat.x * vec.z, quat.x * vec.y - quat.y * vec.x), -(quat.x * vec.x + quat.y * vec.y + quat.z * vec.z));
	}

	constexpr inline glm::vec3 multiply_vec3_quat(const glm::vec3& vec, const glm::vec4& quat) {
		return glm::vec3(quat.w * vec + glm::vec3(vec.y * quat.z - vec.z * quat.y, vec.z * quat.x - vec.x * quat.z, vec.x * quat.y - vec.y * quat.x));
	}

	inline glm::vec4 quat_from_angles_vec4(const glm::vec3& angles) {
		glm::vec4 local[3]{
			{sinf(angles.x / 2),	0,					0,					cosf(angles.x / 2)},
			{0,						sinf(angles.y / 2), 0,					cosf(angles.y / 2)},
			{0,						0,					sinf(angles.z / 2), cosf(angles.z / 2)}
		};
		//local[2].x = 0;
		//local[2].y = 0;
		//local[2].z = sinf(angles.z / 2);
		//local[2].w = cosf(angles.z / 2);

		//local[1].x = 0;
		//local[1].y = sinf(angles.y / 2);
		//local[1].z = 0;
		//local[1].w = cosf(angles.y / 2);

		//local[0].x = sinf(angles.x / 2);
		//local[0].y = 0;
		//local[0].z = 0;
		//local[0].w = cosf(angles.x / 2);

		return multiply_quats(multiply_quats(local[2], local[1]), local[0]);
	}

	inline glm::vec3 rotation_by_quat(const glm::vec3& pos, const glm::vec3& angles) {
		glm::vec4 rot = quat_from_angles_vec4(angles);
		return multiply_quats_to_vec3(multiply_quat_vec3(rot, pos), glm::vec4(-rot.x, -rot.y, -rot.z, rot.w));
	}

	constexpr inline glm::vec3 rotation_by_quat(const glm::vec3& pos, const glm::vec4& quat) {
		return multiply_quats_to_vec3(multiply_quat_vec3(quat, pos), glm::vec4(-quat.x, -quat.y, -quat.z, quat.w));
	}

	inline glm::vec3 rotateX(const glm::vec3& pos, float angle)
	{
		float y = sinf(angle);
		float w = cosf(angle);
		return glm::vec3(pos.x,
			w * pos.y - y * pos.z,
			y * pos.y + w * pos.z
		);
	}

	inline glm::vec3 rotateY(const glm::vec3& pos, float angle)
	{
		float y = sinf(angle);
		float w = cosf(angle);
		return glm::vec3(w * pos.x + y * pos.z,
			pos.y,
			w * pos.z - y * pos.x
		);
	}

	inline glm::vec3 rotateZ(const glm::vec3& pos, float angle)
	{
		float y = sinf(angle);
		float w = cosf(angle);
		return glm::vec3(w * pos.x - y * pos.y,
			y * pos.x + w * pos.y,
			pos.z
		);
	}

	constexpr inline glm::mat4 ortho(const double left, const double right, const double bottom, const double top, const double nearVal, const double farVal)
	{
		return glm::mat4(2.0 / (right - left), 0, 0, (right + left) / (left - right),
			0, 2.0 / (top - bottom), 0, (top + bottom) / (bottom - top),
			0, 0, 2.0 / (nearVal - farVal), (farVal + nearVal) / (nearVal - farVal),
			0, 0, 0, 1.0);
	}

	constexpr inline glm::mat4 projectionMatrix(float inverseTanHalfFovy, float inversedAspect, float zNear, float zFar)
	{
		return glm::mat4{
			inversedAspect * inverseTanHalfFovy, 0,							0,								0,
			0,									 -inverseTanHalfFovy,		0,								0,
			0,									 0,							zFar / (zNear - zFar),			-1.0f,
			0,									 0,							zFar * zNear / (zNear - zFar),	0
		};
	}

	constexpr inline glm::mat4 lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
	{
		const glm::vec3 f(fast_normalize_glm_vec3(center - eye));
		const glm::vec3 s(fast_normalize_glm_vec3(cross(f, up)));
		const glm::vec3 u(cross(s, f));

		//mat<4, 4, T, Q> Result(1);
		//Result[0][0] = s.x;
		//Result[1][0] = s.y;
		//Result[2][0] = s.z;
		//Result[0][1] = u.x;
		//Result[1][1] = u.y;
		//Result[2][1] = u.z;
		//Result[0][2] = -f.x;
		//Result[1][2] = -f.y;
		//Result[2][2] = -f.z;
		//Result[3][0] = -dot(s, eye);
		//Result[3][1] = -dot(u, eye);
		//Result[3][2] = dot(f, eye);
		return glm::mat4{
			s.x,			u.x,			-f.x,			0,
			s.y,			u.y,			-f.y,			0,
			s.z,			u.z,			-f.z,			0,
			-dot(s, eye),	-dot(u, eye),	dot(f, eye),	1
		};
	}
}

#endif