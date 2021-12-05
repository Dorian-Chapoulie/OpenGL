#pragma once
#include "Mesh.h"

struct ModelData
{
	std::vector<Texture> loadedTextures;
	std::vector<Mesh*> meshes;
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
	};

	[[nodiscard]] ModelData* loadModel(const std::string& path, Type&& type);
};

