#include "ModelLoader.h"
#include "DefaultLoader.h"

ModelLoader* ModelLoader::instance = nullptr;

ModelLoader::ModelLoader()
{
}

ModelLoader* ModelLoader::getInstance()
{
	if (instance == nullptr) instance = new ModelLoader();
	return instance;
}

ModelData* ModelLoader::loadModel(const std::string& path, Type&& type)
{
	switch (type)
	{
	case DEFAULT:
		return DefaultLoader().loadModel(path);
		break;
	default:
		return DefaultLoader().loadModel(path);
		break;
	}
}
