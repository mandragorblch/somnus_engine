#include "two_d_object.h"

two_d_object::two_d_object(std::string texture_path, class vk* p_vk) : texture_path(texture_path), p_vk(p_vk)
{
	objectIndex = p_vk->twoDObjects.size();
	vertices = new std::vector<ui_Vertex>;
	p_vk->twoDObjects.push_back(this);
}

two_d_object::two_d_object(std::string texture_path, std::vector<ui_Vertex>* vertices, std::vector<uint32_t> indices, twoDObjectInfo info, class vk* p_vk) : texture_path(texture_path), vertices(vertices), indices(indices), info(info), p_vk(p_vk)
{
	objectIndex = p_vk->twoDObjects.size();
	p_vk->twoDObjects.push_back(this);
}

two_d_object::two_d_object(class vk* p_vk) : p_vk(p_vk)
{
	objectIndex = p_vk->twoDObjects.size();
	vertices = new std::vector<ui_Vertex>;
	p_vk->twoDObjects.push_back(this);
}

two_d_object::two_d_object()
{
	vertices = new std::vector<ui_Vertex>;
}
