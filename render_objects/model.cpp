#include "model.h"

model::model(std::string const& path, class vk* p_vk, bool gamma) : gammaCorrection(gamma), p_vk(p_vk)
{
	filename = path;
	id = p_vk->model_counter;
	first_mesh_id = p_vk->all_meshes.size();
	loadModel(path);
}

model::model(class vk* p_vk) : p_vk(p_vk)
{
}

std::map<std::string, BoneInfo>& model::GetBoneInfoMap()
{
	return m_BoneInfoMap;
}

int model::GetBoneCount()
{
	return m_BoneCounter;
}

void model::loadModel(std::string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices);
	//importer.ApplyPostProcessing(aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_ForceGenNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		assert(0);
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

void model::processNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		class mesh* current_mesh = new class mesh();
		*current_mesh = processMesh(mesh, scene);
		meshes.push_back(current_mesh);
		p_vk->all_meshes.push_back(current_mesh);
		mesh_amount++;
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void model::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.boneIds[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

mesh model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		SetVertexBoneDataToDefault(vertex);
		vertex.pos = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
		vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
		vertex.tangent = (mesh->mTangents) ? AssimpGLMHelpers::GetGLMVec(mesh->mTangents[i]) : glm::vec3(0.0);
		//vertex.bitangent = AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[i]);
		//if (glm::dot(glm::cross(vertex.normal, vertex.tangent), AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[i])) < 0.0f) {
		//	vertex.tangent = -vertex.tangent;
		//}
		//vertex.tangent = rotate(vertex.normal, glm::vec3(vertex.tangent.x, vertex.tangent.y, vertex.tangent.z), glm::radians(95.f));
		//glm::vec3 bitangent = AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[i]);

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec;
		} else {
			vertex.texCoord = glm::vec2(0.0f, 0.0f);
		}

		//if ((i + 1) % 3 == 0) {
		//	glm::vec3 pos1 = vertex.pos;
		//	glm::vec3 pos2 = vertices[i - 1].pos;
		//	glm::vec3 pos3 = vertices[i - 2].pos;
		//	glm::vec2 uv1 = vertex.texCoord;
		//	glm::vec2 uv2 = vertices[i - 1].texCoord;
		//	glm::vec2 uv3 = vertices[i - 2].texCoord;
		//	glm::vec3 edge1 = pos2 - pos1;
		//	glm::vec3 edge2 = pos3 - pos1;
		//	glm::vec2 deltaUV1 = uv2 - uv1;
		//	glm::vec2 deltaUV2 = uv3 - uv1;
		//	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		//	glm::vec3 new_tangent;
		//	new_tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		//	new_tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		//	new_tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		//	glm::vec3 bitangent;
		//	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		//	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		//	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		//	if (glm::dot(glm::cross(new_tangent, bitangent), AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i])) < 0.0f) {
		//		new_tangent = new_tangent * -1.0f;
		//	}
		//	glm::vec3 new_normal = glm::vec3(glm::cross(new_tangent, bitangent));
		//	vertex.normal = new_normal;
		//	vertices[i - 1].normal = new_normal;
		//	vertices[i - 2].normal = new_normal;
		//}
		//vertex.pos = custom_maths::rotateY(vertex.pos, glm::radians(30.f));
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	Texture diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	Texture normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
	//std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
	ExtractBoneWeightForVertices(vertices, mesh, scene);

	return class mesh(vertices, indices, diffuseMaps, normalMaps);
}

void model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.boneIds[i] < 0)
		{
			vertex.weights[i] = weight;
			vertex.boneIds[i] = boneID;
			break;
		}
	}
}

void model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	auto& boneInfoMap = m_BoneInfoMap;
	int& boneCount = m_BoneCounter;

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo(boneCount, AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix));
			boneInfoMap.insert(std::pair(boneName, newBoneInfo));
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

Texture model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	Texture textures(-1);
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++){
		if (i == 1) { assert(0); }
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string path = str.C_Str();
		std::string filename{};
		for (int i = path.size(); i != -1; i--) {
			if (path[i] == '\\' || path[i] == '/') {
				break;
			}
			filename.insert(0, 1, path[i]);
		}

		if (typeName == "texture_diffuse") {
			textures = (*p_vk->all_texures[p_vk->addUniqueTexture(filename, typeName, p_vk)]);
		} else if(typeName == "texture_normal") {
			textures = (*p_vk->all_normal_maps[p_vk->addUniqueTexture(filename, typeName, p_vk)]);
		}
	}
	return textures;
}
