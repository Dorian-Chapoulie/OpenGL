#pragma once
#include <assimp/scene.h>
#include "ILoader.h"
#include <array>

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

	//LOWERCASE ONLY
	const std::array<std::string, 25> bonesHitboxNames = {
		"hips",
		"spine",
		"spine1",
		"spine2",
		"neck",
		"head",
		"headtop_end",

		"leftshoulder",
		"leftarm",
		"leftforearm",
		"lefthand",

		"rightshoulder",
		"rightarm",
		"rightforearm",
		"righthand",

		"leftupleg",
		"leftleg",
		"leftfoot",
		"lefttoebase",
		"lefttoe_end",

		"rightupleg",
		"rightleg",
		"rightfoot",
		"righttoebase",
		"righttoe_end",
	};

};

