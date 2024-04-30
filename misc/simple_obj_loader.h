#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include "misc/vk_misc.h"
#include "PhysX/PhysX.h"

bool load_object_file(std::string path, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uv, std::vector<glm::vec3>* normals, std::vector<unsigned int>* indixes);
bool load_object_file(std::string path, std::vector<Vertex>* vertices, std::vector<unsigned int>* indixes);
bool load_collider_from_file(std::string path, std::vector<physx::PxVec3>* vertices/*, std::vector<physx::PxU32>* indixes*/);