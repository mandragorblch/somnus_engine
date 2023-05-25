#include "two_d_object.h"

two_d_object::two_d_object(std::string texture_path) : texture_path(texture_path)
{
    info.pos = { 0, 0 };
    info.rotation = { 0, 0, 0 };

    ui_Vertex topLeftSquare;

    topLeftSquare.pos = {-.5f, -.5f};
    topLeftSquare.texCoord = {0, 0};

    vertices.push_back(topLeftSquare);

    ui_Vertex topRightSquare;

    topLeftSquare.pos = { .5f, -.5f };
    topLeftSquare.texCoord = {1, 0};

    vertices.push_back(topLeftSquare);

    ui_Vertex bottomRightSquare;

    topLeftSquare.pos = { .5f, .5f };
    topLeftSquare.texCoord = {1, 1};

    vertices.push_back(topLeftSquare);

    ui_Vertex bottomLeftSquare;

    topLeftSquare.pos = { -.5f, .5f };
    topLeftSquare.texCoord = {0, 1};

    vertices.push_back(topLeftSquare);

    indices = {
        2, 1, 0,
        0, 3, 2
    };
}

two_d_object::two_d_object()
{
}
