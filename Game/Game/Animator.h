#pragma once
#include "Animation.h"
#include "Model.h"

class Animator
{
public:
	Animator(Animation* Animation, Model* model);

	void UpdateAnimation(float dt);

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> GetFinalBoneMatrices();

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

	Model* model = nullptr;

	void updateBoneHitbox(const glm::mat4& boneTransformation, const std::string_view& boneName) const;
};
