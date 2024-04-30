#include "create_text_object.h"

std::vector<glm::vec3> calc_standart_vertices_for_debug(float textSizeX, float textSizeY)
{
	return std::vector<glm::vec3>({
		{-1.f,			   -1.f,			 0.f},
		{-1.f + textSizeX, -1.f,			 0.f},
		{-1.f + textSizeX, -1.f + textSizeY, 0.f},
		{-1.f,			   -1.f + textSizeY, 0.f},
		});
}

std::vector<unsigned int> indices = {
	2, 1, 0,
	0, 3, 2
};

std::vector<glm::vec2> uv = {
	{ 0, 0 },
	{ 1, 0 },
	{ 1, 1 },
	{ 0, 1 }
};
//topLeftSquare.pos = { -1 * relative_width, -1 * relative_height };
//topLeftSquare.texCoord = ;
//
//topRightSquare.pos = { relative_width, -1 * relative_height };
//topRightSquare.texCoord = { 1, 0 };
//
//bottomRightSquare.pos = { relative_width, relative_height };
//bottomRightSquare.texCoord = { 1, 1 };
//
//bottomLeftSquare.pos = { -1 * relative_width, relative_height };
//bottomLeftSquare.texCoord = { 0, 1 };

void textElements(float x, float y, std::vector<std::string>& elements, window_class* mWindow, two_d_object* current_two_d_object)
{
	float size_of_texture_for_16px = 1.f / 32.f;
	float size_of_texture_for_8px = 1.f / 64.f;
	float normalTextSize = .12f;
	float textSizeX = normalTextSize;
	float textSizeY = normalTextSize;
	float x_texture_for_debug;
	float y_texture_for_debug;
	float shift = normalTextSize;

	int offset = 0;
	if (!current_two_d_object->vertices->empty()) {
		current_two_d_object->vertices->clear();
		current_two_d_object->indices = std::vector<uint32_t>();
	}
	for (int i = 0; i < (elements).size(); i++) {
		for (int k = 0; k < (elements)[i].size(); k++) {
			switch ((elements)[i][k]) {
			case ' ':
				continue;
			}
			int textureIndex = 0;
			for (char symb : "abcdefghijklmnopqrstuvwxyz") {
				if ((elements)[i][k] == symb) {
					textureIndex = (elements)[i][k] - 65;
				}
			}
			for (char symb : "1234567890") {
				if ((elements)[i][k] == symb) {
					textureIndex = (elements)[i][k] + 80;
				}
			}
			for (char symb : "ABCDEFGHIJKLMNOPQRSTUVWXYZ") {
				if ((elements)[i][k] == symb) {
					textureIndex = (elements)[i][k] - 65;
				}
			}
			for (char symb : "-") {
				if ((elements)[i][k] == symb) {
					textureIndex = 160;
				}
			}
			if ((elements)[i][k] == '.') {
				textureIndex = 161;
			}
			float y_texture = (textureIndex / 32) * size_of_texture_for_8px;
			float x_texture = (textureIndex % 32) * size_of_texture_for_8px;

			if (mWindow->WIDTH > mWindow->HEIGHT) {
				textSizeX = normalTextSize / ((float)mWindow->WIDTH / mWindow->HEIGHT);
				textSizeY = normalTextSize;
				shift = normalTextSize / ((float)mWindow->WIDTH / mWindow->HEIGHT);
			}
			else {
				textSizeX = normalTextSize;
				textSizeY = normalTextSize / ((float)mWindow->HEIGHT / mWindow->WIDTH);
				shift = normalTextSize / ((float)mWindow->HEIGHT / mWindow->WIDTH);
			}

			std::vector<glm::vec3> standart_vertices_for_debug = calc_standart_vertices_for_debug(textSizeX, textSizeY);
			for (int vert = 0; vert < 4; vert++) {
				ui_Vertex new_uiVertex;
				new_uiVertex.pos = glm::vec2(standart_vertices_for_debug[vert][0] + (shift * offset) + x, standart_vertices_for_debug[vert][1] + y);
				new_uiVertex.texCoord = glm::vec2(uv[vert][0] * size_of_texture_for_8px + x_texture, uv[vert][1] * size_of_texture_for_8px + y_texture);
				current_two_d_object->vertices->push_back(new_uiVertex);
			}
			int indices_size = current_two_d_object->indices.size();
			std::vector<unsigned int> local_index;
			for (int ind = 0; ind < 6; ind++) {
				current_two_d_object->indices.push_back(indices[ind] + indices_size / 6 * 4);
			}
			offset++;
		}
		offset++;
	}

	current_two_d_object->texture_path = "atlasShrift.png";
}