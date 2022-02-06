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
		float sizeRadius = 0.0f;
		float sizeHeight = 0.0f;
	};

	static inline const std::array<BoneHierarchy, 20> bonesHitboxNames = { {
			/*0*/{"leftshoulder", 11, true, -0.2f},
			/*1*/{"leftarm", 0},
			/*2*/{"leftforearm", 1},
			/*3*/{"lefthand", 2},

			/*4*/{"rightshoulder", 11, true, -0.2f},
			/*5*/{"rightarm", 4},
			/*6*/{"rightforearm", 5},
			/*7*/{"righthand", 6},

			/*8*/{"hips", -1, true, 0.0f, -0.3f},
			/*9*/{"spine", 8, true, 0.0f, -0.3f},
			/*10*/{"spine1", 9, true, 0.0f, -0.3f},
			/*11*/{"spine2", 10, true, 0.0f, -0.3f},

			/*12*/{"rightupleg", -1, false, 0.1f, -1.8f},
			/*13*/{"rightleg", 12, false, 0.1f, -1.8f},
			/*14*/{"rightfoot", 13, false, 0.0f, -0.8f},

			/*15*/{"leftupleg", -1, false, 0.1f, -1.8f},
			/*16*/{"leftleg", 15, false, 0.1f, -1.8f},
			/*17*/{"leftfoot", 16, false, 0.0f, -0.8f},

			/*18*/{"neck", 11, false, 0.2f, -0.4f},
			/*19*/{"head", 18, false, 0.3f, 0.0f},
		} };
	static std::map<std::string, SkeletalModelData> cache;

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

