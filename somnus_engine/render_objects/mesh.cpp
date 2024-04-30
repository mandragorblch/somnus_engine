#include "mesh.h"

mesh::mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture textures, Texture normal_maps): vertices(vertices), indices(indices), textures(textures), normal_maps(normal_maps)
{
}
