#include "ModelLoader.h"
#include "DefaultLoader.h"
#include "SkeletalLoader.h"

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
	case SKELETAL:
		return SkeletalLoader().loadModel(path);
	default:
		return DefaultLoader().loadModel(path);
	}
}
