#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "misc/vk_misc.h"
#include "GLFW/window_class.h"
#include "render_objects/two_d_object.h"

std::vector<glm::vec3> calc_standart_vertices_for_debug(float textSizeX, float textSizeY);

void textElements(float x, float y, std::vector<std::string>& elements, class window_class* mWindow, class two_d_object* current_two_d_object);
