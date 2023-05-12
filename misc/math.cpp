#include "math.h"

glm::mat3 rotateX(float angle)
{
	return glm::mat3(1, 0, 0,
		0, cos(angle), -sin(angle),
		0, sin(angle), cos(angle));
}

glm::mat3 rotateY(float angle)
{
	return glm::mat3(cos(angle), 0, sin(angle),
		0, 1, 0,
		-sin(angle), 0, cos(angle));
}

glm::mat3 rotateZ(float angle)
{
	return glm::mat3(cos(angle), -sin(angle), 0,
		sin(angle), cos(angle), 0,
		0, 0, 1);
}

glm::mat4 ortho(double left, double right, double bottom, double top, double nearVal, double farVal)
{
	return glm::mat4(2.0 / (right - left), 0, 0, (right + left) / (left - right),
		0, 2.0 / (top - bottom), 0, (top + bottom) / (bottom - top),
		0, 0, 2.0 / (nearVal - farVal), (farVal + nearVal) / (nearVal - farVal),
		0, 0, 0, 1.0);
}