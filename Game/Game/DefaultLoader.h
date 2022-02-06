#pragma once
#include <assimp/scene.h>

#include "ILoader.h"
#include "Mesh.h"

class DefaultLoader :
	public ILoader
{
public:
	[[nodiscard]] ModelData* loadModel(const std::string& path) override;
	static std::map<std::string, ModelData> cache;

private:

	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	std::string directory;
	ModelData* data = new ModelData();
};

