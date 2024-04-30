#include "button.h"

button::button(glm::vec2 relative_pos, double relative_width, double relative_height, std::string texture_path, std::string button_name, vk* p_vk) : pos(relative_pos), origin_width(relative_width), origin_height(relative_height), path(texture_path), name(button_name), p_vk(p_vk){
    //creating 2d obj
    button_object = new class two_d_object(p_vk);
    {
        button_object->info.pos = relative_pos;
        button_object->info.rotation = { 0, 0, 0 };
        button_object->info.is_interacted = false;
        button_object->info.texture_index = 0;

        ui_Vertex topLeftSquare;
        ui_Vertex topRightSquare;
        ui_Vertex bottomRightSquare;
        ui_Vertex bottomLeftSquare;

        topLeftSquare.pos = { -1 * relative_width, -1 * relative_height };
        topLeftSquare.texCoord = { 0, 0 };

        topRightSquare.pos = { relative_width, -1 * relative_height };
        topRightSquare.texCoord = { 1, 0 };

        bottomRightSquare.pos = { relative_width, relative_height };
        bottomRightSquare.texCoord = { 1, 1 };

        bottomLeftSquare.pos = { -1 * relative_width, relative_height };
        bottomLeftSquare.texCoord = { 0, 1 };

        origin_vertices.push_back(topLeftSquare);
        origin_vertices.push_back(topRightSquare);
        origin_vertices.push_back(bottomRightSquare);
        origin_vertices.push_back(bottomLeftSquare);

        aspect_affected_vertices = origin_vertices;

        button_object->indices = {
            2, 1, 0,
            0, 3, 2
        };

        button_object->vertices = (&aspect_affected_vertices);

        button_object->texture_path = texture_path;

        width = origin_width;
        height = origin_height;
    }
}

button::~button()
{
    delete button_object;
}

void button::change_aspect()
{
    ui_Vertex topLeftSquare;
    ui_Vertex topRightSquare;
    ui_Vertex bottomRightSquare;
    ui_Vertex bottomLeftSquare;

    if(p_vk->mWindow->WIDTH > p_vk->mWindow->HEIGHT) {
        topLeftSquare.pos = { origin_vertices[0].pos[0] / (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT), origin_vertices[0].pos[1]};
        topLeftSquare.texCoord = { 0, 0 };

        topRightSquare.pos = { origin_vertices[1].pos[0] / (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT), origin_vertices[1].pos[1] };
        topRightSquare.texCoord = { 1, 0 };

        bottomRightSquare.pos = { origin_vertices[2].pos[0] / (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT), origin_vertices[2].pos[1] };
        bottomRightSquare.texCoord = { 1, 1 };

        bottomLeftSquare.pos = { origin_vertices[3].pos[0] / (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT), origin_vertices[3].pos[1] };
        bottomLeftSquare.texCoord = { 0, 1 };

        width = origin_width / (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT);
        height = origin_height;
    }
    else {
        topLeftSquare.pos = { origin_vertices[0].pos[0], origin_vertices[0].pos[1] * (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT) };
        topLeftSquare.texCoord = { 0, 0 };

        topRightSquare.pos = { origin_vertices[1].pos[0], origin_vertices[1].pos[1] * (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT) };
        topRightSquare.texCoord = { 1, 0 };

        bottomRightSquare.pos = { origin_vertices[2].pos[0], origin_vertices[2].pos[1] * (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT) };
        bottomRightSquare.texCoord = { 1, 1 };

        bottomLeftSquare.pos = { origin_vertices[3].pos[0], origin_vertices[3].pos[1] * (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT) };
        bottomLeftSquare.texCoord = { 0, 1 };

        width = origin_width;
        height = origin_height * (p_vk->mWindow->WIDTH / (double)p_vk->mWindow->HEIGHT);
    }

    aspect_affected_vertices[0] = topLeftSquare;
    aspect_affected_vertices[1] = topRightSquare;
    aspect_affected_vertices[2] = bottomRightSquare;
    aspect_affected_vertices[3] = bottomLeftSquare;
}

void button::calculate_pixel_pos()
{
    pixel_pos_top_right = { (width + 1 + pos[0]) * p_vk->mWindow->WIDTH / 2., (height + 1 + pos[1]) * p_vk->mWindow->HEIGHT / 2. };
    pixel_pos_bottom_left = { (-1 * width + 1 + pos[0]) * p_vk->mWindow->WIDTH / 2., (-1 * height + 1 + pos[1]) * p_vk->mWindow->HEIGHT / 2. };
}
