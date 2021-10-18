#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Bullet3/btBulletCollisionCommon.h>
#include <Bullet3/btBulletDynamicsCommon.h>

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class Model {
public:
	Model(const std::string& path, const glm::vec3& position, float weight, bool hasHitbox, bool hasMultipleHitboxes);
	Model(const std::string& path, const glm::vec3& position, float weight, bool hasHitbox);
	Model(const std::string& path, const glm::vec3& position, bool hasHitbox);
	~Model();
	void draw(Shader& shader);

	void setPosition(const glm::vec3& position);
	void setRotationAroundCenter(const float angle);
	void setWorldTransform(const glm::vec3& position, const glm::quat& rotation);

	glm::vec3 getPosition();
	const glm::vec3 getCenter() const;
	const glm::vec3 getSize() const;
	const glm::vec3 getBasePosition() const;
	const float getCenterRotation() const;
	
	std::vector<btRigidBody*> getRigidBodys() const;
private:
	//TODO: loadedModels
	glm::vec3 position = glm::vec3(0.0f);
	float centerRotation = 0.0f;
	glm::vec3 basePosition = glm::vec3(0.0f);
	std::vector<Texture> loadedTextures;
	std::vector<Mesh*> meshes;
	std::string directory;

	std::vector<glm::vec3> sizes;
	std::vector<glm::vec3> centers;

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	void setupHitboxes();
	void setupHitbox();
	std::vector<glm::vec3> getMeshCenterAndSize(const std::vector<Vertex>& vertices) const;
	std::vector<glm::vec3> getBiggestHitBox() const;

	bool hasHitbox = true;

//BulletPhysics
private:
	btVector3 m_scaledMeshOffsetToBody;

	std::vector<btRigidBody*> worldTrans;
	std::vector<btCollisionShape*> boxCollisionShapes;
	std::vector<btRigidBody*> rigidBodys;

	float weight = 0.0f;
};

