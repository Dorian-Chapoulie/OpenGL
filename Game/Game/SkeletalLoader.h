#pragma once
#include <assimp/scene.h>
#include "ILoader.h"

class SkeletalLoader :
	public ILoader
{

public:
	[[nodiscard]] ModelData* loadModel(const std::string& path) override;


private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	std::string directory;
	SkeletalModelData* data = new SkeletalModelData();
	const int MAX_BONE_WEIGHTS = 4;
};

