#include "create_shape.h"

mesh* create_cube_mesh(glm::vec3 pos, glm::vec3 rot, float size, unsigned int index, std::string path, vk* p_vk)
{
    mesh* new_mesh = new mesh();
    for (int i = 0; i < 6; i++) {
        Vertex new_vertex;
        for (int k = 0; k < 4; k++) {
            new_vertex.pos = { normal_vertices[i * 4 + k][0] * size / 2 + pos.x, normal_vertices[i * 4 + k][1] * size / 2 + pos.y, normal_vertices[i * 4 + k][2] * size / 2 + pos.z };
            //new_vertex.pos = rotateX(rot[0]) * rotateY(rot[1]) * rotateZ(rot[2]) * new_vertex.pos;
            if (i == 0) {
                new_vertex.normal = glm::vec3(0, 0, -1);
            }
            else if (i == 1) {
                new_vertex.normal = glm::vec3(0, 0, 1);
            }
            else if (i == 2) {
                new_vertex.normal = glm::vec3(-1, 0, 0);
            }
            else if (i == 3) {
                new_vertex.normal = glm::vec3(1, 0, 0);
            }
            else if (i == 4) {
                new_vertex.normal = glm::vec3(0, -1, 0);
            }
            else if (i == 5) {
                new_vertex.normal = glm::vec3(0, 1, 0);
            }
            new_vertex.texCoord = { standart_uv[k][0], standart_uv[k][1] };
            new_vertex.tangent = glm::vec3(0);
            new_vertex.boneIds = glm::ivec4(0);
            new_vertex.weights = glm::vec4(0);
            new_mesh->vertices.push_back(new_vertex);
        }

        int indices_size = new_mesh->indices.size();
        //êâàäðàò ýòî 2 òðåóãîëüíèêà => 6 âåðøèí íàäî íàòÿíóòü
        for (int k = 0; k < 6; k++) {
            new_mesh->indices.push_back(standart_indices[k] + (indices_size / 6) * 4);
        }
    }
    new_mesh->info.index = p_vk->all_meshes.size();
    new_mesh->textures = *p_vk->all_texures[p_vk->addUniqueTexture(path, "texture_diffuse", p_vk)];
    return new_mesh;
}
