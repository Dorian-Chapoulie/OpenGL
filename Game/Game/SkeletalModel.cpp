#include "SkeletalModel.h"

SkeletalModel::SkeletalModel(const std::string_view& path, const glm::vec3& position, const glm::vec3 scale)
	: Model(position, scale)
{
	loadModel(path);
}

std::map<std::string, bone_info>& SkeletalModel::get_bone_info_map()
{
	return data.bone_info_map_;
}

int& SkeletalModel::get_bone_count()
{
	return data.bone_counter_;
}

void SkeletalModel::loadModel(const std::string_view& path)
{
	data = LoaderFactory::getInstance()->load_model(path, LoaderFactory::SKELETON);
}
