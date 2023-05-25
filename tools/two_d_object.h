#pragma once

#include "misc/vk_misc.h"

#include <tiny_obj_loader/tiny_obj_loader.h>

#include <string>
#include <unordered_map>
#include <assert.h>
#include <stdexcept>
#include <vector>

class two_d_object
{
public:
	std::vector<ui_Vertex> vertices;
	std::vector<uint32_t> indices;
	std::string texture_path;
	unsigned int texIndex;
	twoDObjectInfo info;

	two_d_object(std::string texture_path);
	two_d_object();
};