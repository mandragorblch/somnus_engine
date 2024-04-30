#pragma once
#include "render_objects/mesh.h"
#include "vulkan/vk.h"

mesh* create_cube_mesh(glm::vec3 pos, glm::vec3 rot, float size, unsigned int index, std::string path, class vk* p_vk);