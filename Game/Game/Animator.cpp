#include "Animator.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "SkeletalLoader.h"

Animator::Animator(Animation* Animation, Model* model)
{
	this->model = model;
	m_CurrentTime = 0.0;
	m_CurrentAnimation = Animation; //currentAnimation;

	m_FinalBoneMatrices.reserve(100);

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
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{

	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;

		updateBoneHitbox(globalTransformation, nodeName);
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}

void Animator::updateBoneHitbox(const glm::mat4& boneTransformation, const std::string_view& boneName) const
{
	return;

	std::string tmpBoneName = std::string(boneName);
	const std::string bannedStr = "mixamorig";
	if (tmpBoneName.find(bannedStr) != std::string::npos) {
		tmpBoneName = tmpBoneName.replace(0, bannedStr.length() + 2, "");
	}
	std::transform(tmpBoneName.begin(), tmpBoneName.end(), tmpBoneName.begin(), ::tolower);
	btRigidBody* hitbox = model->getHitboxFromBoneName(tmpBoneName);
	if (hitbox != nullptr)
	{
		const float modelRotationY = model->getYRotation();
		const glm::vec3 position = model->getBasePosition();
		const glm::vec3 scale = model->getScale();
		const glm::vec3 modelRotation = model->getRotation();
		const glm::mat4 matrix =
			//glm::translate(glm::mat4(1.0f), position)
			/***/ glm::rotate(glm::mat4(1.0f), modelRotationY, modelRotation)
			//* glm::translate(glm::mat4(1.0f), -position)
			* boneTransformation;

		glm::vec3 s;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(matrix, s, rotation, translation, skew, perspective);


		btRigidBody* childHitbox = nullptr;
		bool isHorizontal = false;
		for (int i = 0; i < SkeletalLoader::bonesHitboxNames.size(); i++)
		{
			if (SkeletalLoader::bonesHitboxNames[i].name == tmpBoneName)
			{
				isHorizontal = SkeletalLoader::bonesHitboxNames[i].horizontal;
				for (int j = 0; j < SkeletalLoader::bonesHitboxNames.size(); j++)
				{
					if (SkeletalLoader::bonesHitboxNames[j].parentId == i)
					{
						childHitbox = model->getHitboxFromBoneName(SkeletalLoader::bonesHitboxNames[j].name);
					}
				}
			}
		}

		if (childHitbox)
		{
			btTransform childTransform = childHitbox->getWorldTransform();
			btVector3 childOrigin = childTransform.getOrigin();
			btVector3 currentOrigin = hitbox->getWorldTransform().getOrigin();

			btTransform tr;
			tr.setIdentity();
			const glm::vec3 modelPosition = model->getPosition();
			const glm::vec3* childBasePosition = static_cast<glm::vec3*>(childHitbox->getUserPointer());
			if (!childBasePosition)
			{
				childBasePosition = new glm::vec3(0.0f);
				return;
			}
			const btVector3 vec = btVector3(
				(((translation.x * scale.x + modelPosition.x) + childBasePosition->x) / 2.0f),
				(((translation.y * scale.y + modelPosition.y) + childBasePosition->y) / 2.0f),
				(((translation.z * scale.z + modelPosition.z) + childBasePosition->z) / 2.0f)
			);
			hitbox->setUserPointer(new glm::vec3(
				translation.x * scale.x + modelPosition.x,
				translation.y * scale.y + modelPosition.y,
				translation.z * scale.z + modelPosition.z
			));
			if (isHorizontal)
			{
				glm::quat myquaternion = glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(90.0f), 0));
				tr.setRotation(btQuaternion(myquaternion.x, myquaternion.y, myquaternion.z, myquaternion.w));
			}
			else
			{
				tr.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
			}
			tr.setOrigin(vec);
			hitbox->setWorldTransform(tr);
		}
		else
		{
			btTransform tr;
			tr.setIdentity();
			const glm::vec3 modelPosition = model->getPosition();
			const btVector3 vec = btVector3(
				(translation.x * scale.x + modelPosition.x),
				(translation.y * scale.y + modelPosition.y),
				(translation.z * scale.z + modelPosition.z)
			);
			hitbox->setUserPointer(new glm::vec3(
				translation.x * scale.x + modelPosition.x,
				translation.y * scale.y + modelPosition.y,
				translation.z * scale.z + modelPosition.z
			));
			if (isHorizontal)
			{
				glm::quat myquaternion = glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(90.0f), 0));
				tr.setRotation(btQuaternion(myquaternion.x, myquaternion.y, myquaternion.z, myquaternion.w));
			}
			else
			{
				tr.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
			}
			tr.setOrigin(vec);
			hitbox->setWorldTransform(tr);
		}
	}
}
