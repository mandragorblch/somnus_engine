#pragma once
#include "glm/glm.hpp"

/**
 * \brief ������� �������� ������ ��� X
 *
 * \param angle - ���� � ��������
 */
glm::mat3 rotateX(float angle);//������� �������� ������ ��� X
/**
 * \brief ������� �������� ������ ��� Y
 *
 * \param angle - ���� � ��������
 */
glm::mat3 rotateY(float angle);
/**
 * \brief ������� �������� ������ ��� Z
 *
 * \param angle - ���� � ��������
 */
glm::mat3 rotateZ(float angle);//������� �������� ������ ��� Z
/**
 * \brief ������� ���������������
 * \param left,
 *	right,
 *	bottom,
 *	top,
 *	nearVal,
 *	farVal
 */
glm::mat4 ortho(double left, double right, double bottom, double top, double nearVal, double farVal);//������� ���������������