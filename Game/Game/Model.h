#pragma once
#include <map>
#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <Bullet3/btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>

#include "BoneInfo.hpp"
#include "IHitBox.h"
#include "ModelLoader.h"
#include "Shader.h"

class IHitBox;
class Model {
public:
	Model(const std::string& path, const glm::vec3& position, glm::vec3 scale);
	~Model();

	void draw(Shader& shader);

	virtual void setPosition(const glm::vec3& position);
	virtual IHitBox* getHitBox();
	virtual void setWorldTransform(const glm::vec3& position, const glm::quat& rot, float centerRotation);

	void setCenter(glm::vec3 center);


	glm::vec3 getCenter() const;
	glm::vec3 getPosition();
	const glm::vec3 getBasePosition() const;
	const glm::mat4 getModelMatrix() const;

	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }

	glm::vec3 getScale() const;
	float getWeight() const;
	ModelData* getModelData();

protected:
	Model(const std::string& path, const glm::vec3& position, float weight, glm::vec3 scale);

	ModelData* modelData;

	glm::vec3 center = glm::vec3(0.0f);
	//TODO: loadedModels
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 basePosition = glm::vec3(0.0f);

	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	virtual void loadModel(const std::string& path);

	//BulletPhysics
private:
	float weight = 0.0f;

	//Animation
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;
	const int MAX_BONE_WEIGHTS = 4;

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};

