#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Bone.h"
#include "SkeletalModel.h"

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string_view& animationPath, SkeletalModel* skeletalModel);

	~Animation();

	Bone* FindBone(const std::string& name);

	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string, bone_info>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}
	SkeletalModel* skeletalModel = nullptr;
	glm::vec3 test;
private:
	void ReadMissingBones(const aiAnimation* animation);

	void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, bone_info> m_BoneInfoMap;
};