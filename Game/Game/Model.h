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
	Model(const std::string& path, const glm::vec3& position, float weight, bool hasMultipleHitboxes = true, bool hasHitbox = true);
	Model(const std::string& path, const glm::vec3& position, bool hasMultipleHitboxes = true, bool hasHitbox = true);
	~Model();
	void draw(Shader& shader);

	void setPosition(const glm::vec3& position);
	glm::vec3 getPosition();
	void update();
	
	btRigidBody* getRigidBody() const;
	glm::vec3 position = glm::vec3(0.0f);
private:
	//TODO: loadedModels
	std::vector<Texture> loadedTextures;
	std::vector<Mesh*> meshes;
	std::string directory;

	glm::vec3 size = glm::vec3(0.0f);
	glm::vec3 center = glm::vec3(0.0f);
	
	glm::vec3 orientation = glm::vec3(0.0f);
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
	bool hasMultipleHitboxes = true;

//BulletPhysics
private:
	btVector3 m_scaledMeshOffsetToBody;
	btVector3 m_scaleMeshToBody;

	btTransform transform;
	//btDefaultMotionState* motionState = nullptr;
	btRigidBody* rigidBody = nullptr;
	btCollisionShape* boxCollisionShape = nullptr;

	float weight = 0.0f;
};

