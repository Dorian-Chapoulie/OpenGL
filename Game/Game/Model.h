#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class Model
{
public:
	Model(const std::string& path);
	Model(const std::string& path, const glm::vec3& position);
	void draw(Shader& shader) const;

	void setModelMatrix(const glm::mat4& matrix);
	void setPosision(const glm::vec3& position);

public:
	std::vector<Texture> loadedTextures;
	std::vector<Mesh> meshes;
	std::string directory;

	glm::vec3 position = glm::vec3(0.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
};

