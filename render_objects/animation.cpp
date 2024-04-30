#include "animation.h"

Animation::Animation(const std::string& animationPath, model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	m_Duration = animation->mDuration;
	m_TicksPerSecond = animation->mTicksPerSecond;
	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	ReadHierarchyData(m_RootNode, scene->mRootNode);
	ReadMissingBones(animation, model);
}

Animation::~Animation()
{
}

Bone* Animation::FindBoneName(const std::string& name)
{
	return &m_Bone_name_map[name];
}

Bone* Animation::FindBoneID(uint32_t ID)
{
	return &m_Bone_ID_map[ID];
}

void Animation::ReadMissingBones(const aiAnimation* animation, model* model)
{
	int size = animation->mNumChannels;

	std::map<std::string, struct BoneInfo> boneInfoMap(model->GetBoneInfoMap());//getting m_BoneInfoMap from Model class
	int boneCount = model->GetBoneCount(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo(boneCount, glm::mat4(0));
			boneInfoMap.insert(std::pair(boneName, newBoneInfo));
			boneCount++;
		}
		m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
		m_Bone_ID_map.insert(std::pair(boneInfoMap[channel->mNodeName.data].id, m_Bones.back()));
		m_Bone_name_map.insert(std::pair(m_Bones.back().GetBoneName(), m_Bones.back()));
	}

	m_BoneInfoMap = boneInfoMap;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.ID = counter++;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
