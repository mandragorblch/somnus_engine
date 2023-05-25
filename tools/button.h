#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "tools/two_d_object.h"
#include "misc/vk_misc.h"
#include "src/vk.h"

class button
{
private:
	class vk* p_vk;
public:
	double width;
	double height;
	glm::vec2 pos;
	std::string path;
	two_d_object* button_object = new two_d_object();
	glm::vec2 pixel_pos_top_right;
	glm::vec2 pixel_pos_bottom_left;
	button(glm::vec2 relative_pos, double relative_width, double relative_height, std::string texture_path, class vk* p_vk);
	~button();
	void calculate_pixel_pos();
};

