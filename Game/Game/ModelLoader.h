#pragma once
#include <map>
#include "BoneInfo.hpp"
#include "Mesh.h"

struct ModelData
{
	std::vector<Texture> loadedTextures;
	std::vector<Mesh*> meshes;
};

struct SkeletalModelData : ModelData
{
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	std::map<std::string, int> hitboxesBones;
	int m_BoneCounter = 0;
};

class ModelLoader
{
	ModelLoader();

	static ModelLoader* instance;

public:
	static ModelLoader* getInstance();

	enum Type
	{
		DEFAULT,
		SKELETAL,
	};

	[[nodiscard]] ModelData* loadModel(const std::string& path, Type&& type);
};

