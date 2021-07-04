#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <reactphysics3d/collision/Collider.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "ModelUtility.h"

class Model
{
public:
	Model(const std::string& path, MODEL_TYPE type, const glm::vec3& position, float scale = 1.0f, bool hasHitbox = true);
	~Model();
	void draw(Shader& shader);

	void setPosition(const glm::vec3& position);
	void setOriantation(const glm::vec3& oritantion);
	void setMass(float mass) const;

	reactphysics3d::CollisionBody* getCollisionBody() const;

private:
	std::vector<Texture> loadedTextures;
	//TODO: loadedModels
	std::vector<Mesh*> meshes;
	std::string directory;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 orientation = glm::vec3(0.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	float scale = 1.0f;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	reactphysics3d::CollisionBody* body = nullptr;
	reactphysics3d::BoxShape* hitbox = nullptr;
	reactphysics3d::Collider* collider = nullptr;

	void setupHitbox();
	std::vector<glm::vec3> getMeshCenterAndSize(const std::vector<Vertex>& vertices) const;
	std::vector<glm::vec3> getBiggestHitBox() const;
	
	bool hasHitbox = true;
	MODEL_TYPE type = MODEL_TYPE::COLLISION_BODY;
};

