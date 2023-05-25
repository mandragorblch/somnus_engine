#pragma once

#include "misc/vk_misc.h"

#include <tiny_obj_loader/tiny_obj_loader.h>

#include <string>
#include <unordered_map>
#include <assert.h>
#include <stdexcept>
#include <vector>

class three_d_object
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::string texture_path;
	unsigned int texIndex;
	threeDObjectInfo info;

	three_d_object(std::string object_path, std::string texture_path);
};

