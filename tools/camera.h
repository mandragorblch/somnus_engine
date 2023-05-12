#pragma once
#include <glm/glm.hpp>

class camera
{
private:

public:
	void updateVectors();
	glm::vec3 direction;
	glm::vec3 relative_up;
	glm::vec3 relative_right;

	glm::vec3 norm_z = glm::vec3(0, 0, -1);
	glm::vec3 norm_x = glm::vec3(1, 0, 0);
	glm::vec3 norm_y = glm::vec3(0, 1, 0);

	glm::mat4 rotation;

	float horizontalAngle = 0;
	float verticalAngle = 0;

	float fov;
	glm::vec3 position;
	camera(glm::vec3 position, float fov);

	void rotate(float x, float y, float z);

	glm::mat4 getProjection(float aspect);
	glm::mat4 getView();
};