#pragma once
#include <assimp/material.h>
#include <assimp/scene.h>
#include "ILoader.h"

class DefaultLoader : public ILoader
{
public:
	LoadData loadModel(const std::string_view& path) override;

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
};

