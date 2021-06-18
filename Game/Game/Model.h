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
	void draw(Shader& shader) const;

private:
	std::vector<Texture> loadedTextures;
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
};
