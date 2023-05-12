#include "GLFW/window_class.h"
#include <glm/ext.hpp>

camera::camera(glm::vec3 position, float fov) : position(position), fov(fov), rotation(1.0f)
{
	updateVectors();
}

void camera::updateVectors()
{
	//if use rotate
	//front = glm::vec3(rotation * glm::vec4(0, 0, -1, 1));
	//relative_right = glm::vec3(rotation * glm::vec4(1, 0, 0, 1));
	//relative_up = glm::vec3(rotation * glm::vec4(0, 1, 0, 1));

	//or front(about next line)
	direction = glm::vec3(cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle));
	relative_right = glm::vec3(
		-cos(horizontalAngle),
		0,
		sin(horizontalAngle)
	);
	relative_up = glm::cross(relative_right, direction);
}

void camera::rotate(float x, float y, float z)
{
	rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
	rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, x, glm::vec3(1, 0, 0));

	updateVectors();
}

glm::mat4 camera::getProjection(float aspect)
{
	//float aspect = 1;
	//if (!glfwGetWindowAttrib(Window::mWindow, GLFW_ICONIFIED)) {
	//	aspect = (float)Window::width / (float)Window::height;
	//}
	return glm::perspective(fov, aspect, 0.1f, 1000.0f);
}

glm::mat4 camera::getView()
{
	//return glm::lookAt(position, position + relative_front, relative_up);
	return glm::lookAt(position, position + direction, relative_up);
}