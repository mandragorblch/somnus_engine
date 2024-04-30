#include "misc/simple_obj_loader.h"

bool load_object_file(std::string path, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uv, std::vector<glm::vec3>* normals, std::vector<unsigned int>* indixes)
{
    //time limit for running program in milliseconds(if the file is corrupted)
    int time_limit = 10000;
    int start_time = clock();
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    FILE* file = fopen(("toDo\\" + path + ".obj").c_str(), "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1) {
        std::vector<char> lineHeader(256);
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader.data());
        if (res == EOF || clock() - start_time >= time_limit)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        if (strcmp(lineHeader.data(), "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader.data(), "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader.data(), "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader.data(), "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        vertices->push_back(temp_vertices[vertexIndices[i] - 1]);
        uv->push_back(temp_uvs[uvIndices[i] - 1]);
        normals->push_back(temp_normals[normalIndices[i] - 1]);
        indixes->push_back(i);
    }
    // Close stream if it is not NULL
    if (file)
    {
        if (fclose(file))
        {
            printf("The file 'crt_fopen.c' was not closed\n");
        }
    }
}

bool load_object_file(std::string path, std::vector<Vertex>* vertices, std::vector<unsigned int>* indixes)
{
    //time limit for running program in milliseconds(if the file is corrupted)
    int time_limit = 10000;
    int start_time = clock();
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;
    std::vector< glm::vec3 > temp_tangents;

    FILE* file = fopen((path).c_str(), "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1) {
        std::vector<char> lineHeader(256);
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader.data());
        if (res == EOF || clock() - start_time >= time_limit)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        if (strcmp(lineHeader.data(), "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader.data(), "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv = glm::vec2(uv.x, 1 - uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader.data(), "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader.data(), "tgsp") == 0) {
            glm::vec3 tangent;
            fscanf(file, "%f %f %f\n", &tangent.x, &tangent.y, &tangent.z);
            temp_tangents.push_back(tangent);
        }
        else if (strcmp(lineHeader.data(), "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }
    //glm::vec3 local_vertice, normal, tangent;
    //glm::vec2 uvs;
    //unsigned int indixes;
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        Vertex local_vertice;
        local_vertice.pos = temp_vertices[vertexIndices[i] - 1];
        local_vertice.texCoord = temp_uvs[uvIndices[i] - 1];
        local_vertice.normal = temp_normals[normalIndices[i] - 1];
        if (temp_tangents.size() != 0) {
            local_vertice.tangent = temp_tangents[normalIndices[i] - 1];
        }
        vertices->push_back(local_vertice);
        indixes->push_back(i);
    }
    // Close stream if it is not NULL
    if (file)
    {
        if (fclose(file))
        {
            printf("The file was not closed\n");
        }
    }
}

bool load_collider_from_file(std::string path, std::vector<physx::PxVec3>* vertices/*, std::vector<physx::PxU32>* indixes*/)
{
    std::vector<physx::PxU32> vertexIndices;
    std::vector<physx::PxVec3> temp_vertices;

    FILE* file = fopen((path).c_str(), "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1) {
        std::vector<char> lineHeader(256);
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader.data());
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        if (strcmp(lineHeader.data(), "v") == 0) {
            physx::PxVec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader.data(), "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser : Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
        }
    }
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        vertices->push_back(temp_vertices[vertexIndices[i] - 1]);
        //indixes->push_back(i);
    }
    // Close stream if it is not NULL
    if (file)
    {
        if (fclose(file))
        {
            printf("The file 'crt_fopen.c' was not closed\n");
        }
    }
}