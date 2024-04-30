#include "GLFW/window_class.h"
#include "maths/math.h"
#include <glm/ext.hpp>
#include <chrono>

Ñamera::Ñamera(glm::vec3 position, float fov) : position(position), inverseTanHalfFovy(1.0f / tanf(fov / 2)), projView(new ProjViewMatrix()), projection(custom_maths::projectionMatrix(inverseTanHalfFovy, 0, 0.1f, 1000.f))
{
	updateVectors();
}

void Ñamera::updateVectors()
{
	//or front(about next line)
	float cos_verticalAngle = cosf(verticalAngle);
	float sin_verticalAngle = sinf(verticalAngle);
	float cos_horizontalAngle = cosf(horizontalAngle);
	float sin_horizontalAngle = sinf(horizontalAngle);

	direction = custom_maths::fast_normalize_glm_vec3(glm::vec3(
		cos_verticalAngle * sin_horizontalAngle,
		sin_verticalAngle,
		cos_verticalAngle * cos_horizontalAngle));

	relative_right = custom_maths::fast_normalize_glm_vec3(glm::vec3(
		-cos_horizontalAngle,
		0,
		sin_horizontalAngle
	));

	relative_up = custom_maths::fast_normalize_glm_vec3(custom_maths::cross(relative_right, direction));
}

void Ñamera::updateProjectionAspect(float inversedAspect)
{
	//float aspect = 1;
	//if (!glfwGetWindowAttrib(Window::mWindow, GLFW_ICONIFIED)) {
	//	aspect = (float)Window::width / (float)Window::height;
	//}
	//return glm::perspective(fov, aspect, 0.1f, 1000.0f);
	projection[0][0] = inversedAspect * inverseTanHalfFovy;
}

glm::mat4 Ñamera::getView()
{
	//return glm::lookAt(position, position + relative_front, relative_up);
	//return custom_maths::lookAt(position, position + direction, relative_up);
	return glm::mat4{
		relative_right.x,								relative_up.x,									-direction.x,							0,
		relative_right.y,								relative_up.y,									-direction.y,							0,
		relative_right.z,								relative_up.z,									-direction.z,							0,
		-custom_maths::dot(relative_right, position),	-custom_maths::dot(relative_up, position),		custom_maths::dot(direction, position),	1
	};
}

void Ñamera::updateProjViewMatrix()
{
	//static long long amount = 0;
	//static long double time = 0;
	//std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	float zero_zero			=	projection[0][0];
	float one_one			=	projection[1][1];
	float neg_two_two		=	-projection[2][2];
	float neg_two_three		=	-projection[2][3];
	float neg_dot_dir_pos	=	-custom_maths::dot(direction, position);
	projView->projView = glm::mat4{
		zero_zero * relative_right.x,								one_one * relative_up.x,								neg_two_two * direction.x,						neg_two_three *	direction.x,
		0,															one_one * relative_up.y,								neg_two_two * direction.y,						neg_two_three * direction.y,
		zero_zero * relative_right.z,								one_one * relative_up.z,								neg_two_two * direction.z,						neg_two_three * direction.z,
		zero_zero * -custom_maths::dot(relative_right, position),	one_one * -custom_maths::dot(relative_up, position),	neg_two_two * neg_dot_dir_pos - neg_two_three,	neg_two_three * neg_dot_dir_pos
	};
	//time += (std::chrono::high_resolution_clock::duration(std::chrono::high_resolution_clock::now() - startTime)).count();
	//amount++;
	//std::cout << "\033[38;2;255;255;0m" << time / amount << "\033[0m\n";
	//return projection * getView();
}

Ñamera::~Ñamera()
{
	delete projView;
}