#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "misc/vk_misc.h"
#include "bone.h"
#include <functional>
#include "model.h"

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	uint32_t ID;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, class model* model);

	~Animation();

	Bone* FindBoneName(const std::string& name);
	Bone* FindBoneID(uint32_t ID);


	inline int GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline std::map<std::string, BoneInfo>& GetBoneInfoMap()
	{
		return m_BoneInfoMap;
	}
	inline std::map<std::string, Bone>& GetBoneNameMap()
	{
		return m_Bone_name_map;
	}
	inline std::map<uint32_t, Bone>& GetBoneIDMap()
	{
		return m_Bone_ID_map;
	}

private:
	void ReadMissingBones(const aiAnimation* animation, class model* model);

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	float m_Duration;
	int m_TicksPerSecond;
	uint32_t counter = 0;
	std::vector<Bone> m_Bones;
	std::map<std::string, Bone> m_Bone_name_map;
	std::map<uint32_t, Bone> m_Bone_ID_map;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};
