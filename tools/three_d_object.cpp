#include "three_d_object.h"

void three_d_object::cubeVerticesAtCoordinates(glm::vec3 pos, float size)
{
    for (int i = 0; i < 6; i++) {
        Vertex new_vertex;
        for (int k = 0; k < 4; k++) {
            new_vertex.pos = { normal_vertices[i * 4 + k][0] * size / 2 + pos.x, normal_vertices[i * 4 + k][1] * size / 2 + pos.y, normal_vertices[i * 4 + k][2] * size / 2 + pos.z };
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
            new_vertex.index = index;
            vertices.push_back(new_vertex);
        }
        

        int indices_size = indices.size();
        //êâàäðàò ýòî 2 òðåóãîëüíèêà => 6 âåðøèí íàäî íàòÿíóòü
        for (int k = 0; k < 6; k++) {
            indices.push_back(standart_indices[k] + (indices_size / 6) * 4);
        }
    }
}

void three_d_object::cubeVerticesAtCoordinates(glm::vec3 pos, float size, float index)
{
    for (int i = 0; i < 6; i++) {
        Vertex new_vertex;
        for (int k = 0; k < 4; k++) {
            new_vertex.pos = { normal_vertices[i * 4 + k][0] * size / 2 + pos.x, normal_vertices[i * 4 + k][1] * size / 2 + pos.y, normal_vertices[i * 4 + k][2] * size / 2 + pos.z };
            new_vertex.texCoord = { standart_uv[k][0], standart_uv[k][1] };
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
            new_vertex.index = index;
            vertices.push_back(new_vertex);
        }


        int indices_size = indices.size();
        //êâàäðàò ýòî 2 òðåóãîëüíèêà => 6 âåðøèí íàäî íàòÿíóòü
        for (int k = 0; k < 6; k++) {
            indices.push_back(standart_indices[k] + (indices_size / 6) * 4);
        }
    }
}

three_d_object::three_d_object(std::string model_path, std::string texture_path, std::string normal_map) : textures_paths(texture_path), normal_map(normal_map)
{
    //tinyobj::attrib_t attrib;
    //std::vector<tinyobj::shape_t> shapes;
    //std::vector<tinyobj::material_t> materials;
    //std::string warn, err;
    //
    //info.pos = { 0, 0, 0 };
    //info.rotation = { 0, 0, 0 };
    //info.shininess = 0;
    //
    //if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str())) {
    //    assert(0);
    //    throw std::runtime_error(warn + err);
    //}
    //
    //std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    //
    //for (const auto& shape : shapes) {
    //    for (const auto& index : shape.mesh.indices) {
    //        Vertex vertex{};
    //
    //        vertex.pos = {
    //            attrib.vertices[3 * index.vertex_index + 0],
    //            attrib.vertices[3 * index.vertex_index + 1],
    //            attrib.vertices[3 * index.vertex_index + 2]
    //        };
    //
    //        vertex.texCoord = {
    //            attrib.texcoords[2 * index.texcoord_index + 0],
    //            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
    //        };
    //
    //        vertex.normal = {
    //            attrib.normals[3 * index.normal_index + 0],
    //            attrib.normals[3 * index.normal_index + 1],
    //            attrib.normals[3 * index.normal_index + 2]
    //        };
    //        vertex.index = three_d_object::index;
    //        if (uniqueVertices.count(vertex) == 0) {
    //            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
    //            vertices.push_back(vertex);
    //        }
    //        indices.push_back(uniqueVertices[vertex]);
    //    }
    //}
    has_normal_map = true;
    load_object_file(model_path, &vertices, &indices);
}

three_d_object::three_d_object(std::string model_path, std::string texture_path) : textures_paths(texture_path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    info.pos = { 0, 0, 0 };
    info.rotation = { 0, 0, 0 };

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str())) {
        assert(0);
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.normal = { 
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2] 
            };
            vertex.index = three_d_object::index;
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

three_d_object::three_d_object(std::string texture_path) : textures_paths(texture_path)
{

}
