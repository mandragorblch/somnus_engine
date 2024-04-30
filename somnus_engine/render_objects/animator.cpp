#include "animator.h"

Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;

	//m_FinalBoneMatrices;//REWRITE THIS SHIT amount must increase int untime by adding new animation

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		StartCalculateBoneTransform(m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData& node, const glm::mat4& parentTransform)
{
	const std::string& nodeName = node.name;
	const glm::mat4* nodeTransform = &node.transformation;
	//auto startTime = std::chrono::high_resolution_clock::now();
	if(Bone* bone = &(m_CurrentAnimation->GetBoneNameMap())[nodeName]; bone->is_updatable) {
		bone->Update(m_CurrentTime);
		nodeTransform = &bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * (*nodeTransform);

	if(BoneInfo& boneInfo = (m_CurrentAnimation->GetBoneInfoMap())[nodeName]; boneInfo.id >= 0) {
		m_FinalBoneMatrices[boneInfo.id] = globalTransformation * boneInfo.offset;
	}
	//std::cout << std::chrono::high_resolution_clock::duration(std::chrono::high_resolution_clock::now() - startTime).count() << '\n';
	for (int i = 0; i < node.childrenCount; i++)
		CalculateBoneTransform(node.children[i], globalTransformation);
}

void Animator::StartCalculateBoneTransform(const AssimpNodeData& node, const glm::mat4& startTransform)
{
	for (int i = 0; i < node.childrenCount; i++)
		CalculateBoneTransform(node.children[i], startTransform);
}

std::vector<glm::mat4>& Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}
