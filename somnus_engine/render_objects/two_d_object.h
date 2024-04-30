#pragma once

#include "misc/vk_misc.h"
#include "vulkan/vk.h"

#include <tiny_obj_loader/tiny_obj_loader.h>

#include <string>
#include <unordered_map>
#include <assert.h>
#include <stdexcept>
#include <vector>

class two_d_object
{
public:
	std::vector<ui_Vertex>* vertices;
	std::vector<uint32_t> indices = std::vector<uint32_t>();
	std::string texture_path;
	unsigned int texIndex;
	twoDObjectInfo info;
	unsigned objectIndex;
	class vk* p_vk;

	two_d_object(std::string texture_path, class vk* p_vk);
	two_d_object(std::string texture_path, std::vector<ui_Vertex>* vertices, std::vector<uint32_t> indices, twoDObjectInfo info, class vk* p_vk);
	two_d_object(class vk* p_vk);
	two_d_object();
};