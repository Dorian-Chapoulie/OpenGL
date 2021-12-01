#include "LoaderFactory.h"
#include "DefaultLoader.h"
#include "SkeletonLoader.h"

LoaderFactory* LoaderFactory::instance = nullptr;

LoaderFactory::LoaderFactory()
{}

LoaderFactory* LoaderFactory::getInstance()
{
	if (instance == nullptr) instance = new LoaderFactory();
	return instance;
}

LoadData LoaderFactory::load_model(const std::string_view& path, LoaderType&& type)
{
	std::cout << "LOADING: " << path << " AS " << type << std::endl;
	switch (type)
	{
	default:
	case DEFAULT:
		return DefaultLoader().loadModel(path);
		break;
	case SKELETON:
		return SkeletonLoader().loadModel(path);
		break;
	}
}
