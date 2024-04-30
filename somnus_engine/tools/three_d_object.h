#pragma once

#include "misc/vk_misc.h"

#include <tiny_obj_loader/tiny_obj_loader.h>
#include "misc/simple_obj_loader.h"

#include <string>
#include <unordered_map>
#include <assert.h>
#include <stdexcept>
#include <vector>

class three_d_object
{
private:
	std::vector<glm::vec3> normal_vertices = { //(from observer)
	{1, -1, -1},//back
	{1, 1, -1},
	{-1, 1, -1},
	{-1, -1, -1},

	{-1, -1, 1},//face
	{-1, 1, 1},
	{1, 1, 1},
	{1, -1, 1},

	{-1, -1, -1},//left
	{-1, 1, -1},
	{-1, 1, 1},
	{-1, -1, 1},

	{1, -1, 1},//right
	{1, 1, 1},
	{1, 1, -1},
	{1, -1, -1},

	{-1, -1, -1},//down
	{-1, -1, 1},
	{1, -1, 1},
	{1, -1, -1},

	{-1, 1, 1},//top
	{-1, 1, -1},
	{1, 1, -1},
	{1, 1, 1},

	};
	std::vector<glm::vec2> standart_uv = {
		{0, 1},
		{0, 0},
		{1, 0},
		{1, 1},
	};
	std::vector<unsigned int> standart_indices = {
		3, 2, 1,//back
		3, 1, 0
	};
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::string textures_paths;
	std::string normal_map;
	unsigned int texIndex;
	float index;
	meshInfo info;
	void cubeVerticesAtCoordinates(glm::vec3 pos, float size);
	void cubeVerticesAtCoordinates(glm::vec3 pos, float size, float index);
	bool has_normal_map = false;

	three_d_object(std::string object_path, std::string texture_path, std::string normal_map_path);
	three_d_object(std::string object_path, std::string texture_path);
	three_d_object(std::string texture_path);
};

