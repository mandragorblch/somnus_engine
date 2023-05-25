#include "button.h"

button::button(glm::vec2 relative_pos, double relative_width, double relative_height, std::string texture_path, vk* p_vk) : pos(relative_pos), width(relative_width), height(relative_height), path(texture_path), p_vk(p_vk){
    //creating 2d obj
    {
        std::vector<ui_Vertex> vertices;
        button_object->info.pos = relative_pos;
        button_object->info.rotation = { 0, 0, 0 };

        ui_Vertex topLeftSquare;

        topLeftSquare.pos = { -1 * relative_width, -1 * relative_height };
        topLeftSquare.texCoord = { 0, 0 };

        vertices.push_back(topLeftSquare);

        ui_Vertex topRightSquare;

        topLeftSquare.pos = { relative_width, -1 * relative_height };
        topLeftSquare.texCoord = { 1, 0 };

        vertices.push_back(topLeftSquare);

        ui_Vertex bottomRightSquare;

        topLeftSquare.pos = { relative_width, relative_height };
        topLeftSquare.texCoord = { 1, 1 };

        vertices.push_back(topLeftSquare);

        ui_Vertex bottomLeftSquare;

        topLeftSquare.pos = { -1 * relative_width, relative_height };
        topLeftSquare.texCoord = { 0, 1 };

        vertices.push_back(topLeftSquare);

        button_object->indices = {
            2, 1, 0,
            0, 3, 2
        };

        button_object->vertices = vertices;

        button_object->texture_path = texture_path;
    }

    p_vk->twoDObjects.push_back(button_object);
}

button::~button()
{
    delete button_object;
}

void button::calculate_pixel_pos()
{
    pixel_pos_top_right = { (width + 1 + pos[0]) * p_vk->mWindow->WIDTH / 2., (height + 1 + pos[1]) * p_vk->mWindow->HEIGHT / 2. };
    pixel_pos_bottom_left = { (-1 * width + 1 + pos[0]) * p_vk->mWindow->WIDTH / 2., (-1 * height + 1 + pos[1]) * p_vk->mWindow->HEIGHT / 2. };
}
