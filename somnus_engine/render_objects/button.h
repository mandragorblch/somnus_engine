#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "render_objects/two_d_object.h"
#include "misc/vk_misc.h"
#include "vulkan/vk.h"

class button
{
private:
	class vk* p_vk;
	std::vector<ui_Vertex> origin_vertices;
	std::vector<ui_Vertex> aspect_affected_vertices;
	double width;
	double height;
public:
	double origin_width;
	double origin_height;
	glm::vec2 pos;
	std::string path;
	std::string name;
	class two_d_object* button_object;
	glm::vec2 pixel_pos_top_right;
	glm::vec2 pixel_pos_bottom_left;
	button(glm::vec2 relative_pos, double relative_width, double relative_height, std::string texture_path, std::string button_name, class vk* p_vk);
	~button();
	void change_aspect();
	void calculate_pixel_pos();
};

