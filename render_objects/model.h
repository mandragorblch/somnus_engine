#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "bone.h"
#include "misc/vk_misc.h"
#include "vulkan/vk.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "assimp_glm_helpers.h"
#include "maths/math.h"
#include <time.h>

class model
{
public:
	// model data 
	std::vector<Texture>    textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<class mesh*> meshes;
	std::string directory;
	std::string filename;
	bool gammaCorrection;
	uint32_t id = 0;
	uint32_t first_mesh_id = 0;
	uint32_t mesh_amount = 0;

	// constructor, expects a filepath to a 3D model.
	model(std::string const& path, class vk* p_vk, bool gamma = false);
	model(class vk* p_vk);

	std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int GetBoneCount();

private:
	class vk* p_vk;

	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(std::string const& path);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);

	void SetVertexBoneDataToDefault(Vertex& vertex);


	class mesh processMesh(aiMesh* mesh, const aiScene* scene);

	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);


	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	Texture loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

