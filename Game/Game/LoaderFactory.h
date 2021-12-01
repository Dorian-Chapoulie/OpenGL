#pragma once
#include <map>

#include "BoneInfo.hpp"
#include "Mesh.h"

struct LoadData
{
	std::vector<Mesh*> meshes;
	std::vector<Texture> loadedTextures;
	std::string_view directory;

	std::map<std::string, bone_info> bone_info_map_;
	int bone_counter_ = 0;
};

class LoaderFactory
{
	LoaderFactory();
	static LoaderFactory* instance;

public:
	static LoaderFactory* getInstance();
	enum LoaderType
	{
		DEFAULT,
		SKELETON,
	};

	LoadData load_model(const std::string_view& path, LoaderType&& type);
};

