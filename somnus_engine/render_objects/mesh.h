#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include "misc/vk_misc.h"

struct Texture {
    int id;
    std::string type;
    std::string path;
};

class mesh {
public:
    // mesh Data
    std::vector<Vertex>     vertices;
    std::vector<uint32_t>   indices;
    Texture                 textures;
    Texture                 normal_maps;
    index_wrapper           info;

    // constructor
    mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture textures, Texture normal_maps);
    mesh() = default;

//private:
};

