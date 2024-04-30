#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"

class Animator
{
public:
	Animator(class Animation* animation);

	void UpdateAnimation(float dt);

	void PlayAnimation(class Animation* pAnimation);

	void CalculateBoneTransform(const struct AssimpNodeData& node, const glm::mat4& parentTransform);

	void StartCalculateBoneTransform(const struct AssimpNodeData& node, const glm::mat4& startTransform);

	std::vector<glm::mat4>& GetFinalBoneMatrices();

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	class Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

};

