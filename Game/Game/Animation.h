#pragma once 

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Model.h"
#include "Bone.h"

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

    Animation(const std::string& animationPath, Model* model);

    ~Animation();

    Bone* FindBone(const std::string& name);

    inline float GetTicksPerSecond() { return m_TicksPerSecond; }
    inline float GetDuration() { return m_Duration; }
    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
    inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return m_BoneInfoMap;
    }
    Model* model = nullptr;
    glm::vec3 test;
private:
    void ReadMissingBones(const aiAnimation* animation, Model& model);

    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};