#pragma once
#include <glm/glm.hpp>

class Ñamera
{
private:

public:
	void updateVectors();
	glm::vec3 direction;
	glm::vec3 relative_up;
	glm::vec3 relative_right;

	const glm::vec3 norm_z{ 0, 0, -1 };
	const glm::vec3 norm_x{ 1, 0, 0 };
	const glm::vec3 norm_y{ 0, 1, 0 };

	struct ProjViewMatrix* const projView = nullptr;
	glm::vec4 quat_rot{ 1, 0, 0, 0 };

	float horizontalAngle = 0;
	float verticalAngle = 0;

	float inverseTanHalfFovy;
	glm::vec3 position;
	Ñamera(glm::vec3 position, float fov);
	glm::mat4 projection;

	void rotate(float x, float y, float z);

	void updateProjectionAspect(float inversedAspect);
	glm::mat4 getView();
	void updateProjViewMatrix();

	~Ñamera();
};