#pragma once
#include <assimp/scene.h>
#include "ILoader.h"
#include <array>

class SkeletalLoader :
	public ILoader
{

public:
	[[nodiscard]] ModelData* loadModel(const std::string& path) override;

	struct BoneHierarchy
	{
		std::string name;
		int parentId = -1;
		bool horizontal = false;
	};

	static inline const std::array<BoneHierarchy, 25> bonesHitboxNames = { {
		{ "leftshoulder", -1, true},
		{"leftarm_", 0},
		{"leftforearm_", 1},
		{"lefthand_", 2},

		{ "rightshoulder", -1, true},
		{ "rightarm_", 4},
		{ "rightforearm_", 5},
		{ "righthand_", 6},

		{"hips", -1, true},
		{"spine", 8, true},
		{"spine1", 9, true},
		{"spine2", 10, true},
		//{"neck", 8},//10
		//{"head", 9}//11
		/*{"spine1", 9},
		{"spine2", 10},
		{"neck", 11},
		{"head", 12},
		{ "headtop_end", 13},*/
	} };

	/*
	 *
	 *	{"hips_"},
		{"spine_", 0},
		{"spine1_", 1},
		{"spine2_", 2},
		{"neck_", 3},
		{"head_", 4},
		{ "headtop_end_", 5 },

		{ "leftshoulder_", 3 },
		{ "leftarm", 8 },
		{ "leftforearm", 9 },
		{ "lefthand", 10 },

		{ "rightshoulder_", 3 },
		{ "rightarm_", 11 },
		{ "rightforearm_", 12 },
		{ "righthand_", 13 },

		{ "leftupleg_", 0 },
		{ "leftleg_", 15 },
		{ "leftfoot_", 16 },
		{ "lefttoebase_", 17 },
		{ "lefttoe_end_", 18 },

		{ "rightupleg_", 0 },
		{ "rightleg_", 20 },
		{ "rightfoot_", 21 },
		{ "righttoebase_", 22 },
		{ "righttoe_end_", 23 },
	 *
	 */
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
	/*const std::array<std::string, 25> bonesHitboxNames = {
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
	};*/
};

