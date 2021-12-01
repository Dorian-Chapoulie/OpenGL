#pragma once
#include "Model.h"
#include "BoneInfo.hpp"

class SkeletalModel :
	public Model
{
public:

	SkeletalModel(const std::string_view& path, const glm::vec3& position, const glm::vec3 scale);

	[[nodiscard]] std::map<std::string, bone_info>& get_bone_info_map();
	[[nodiscard]] int& get_bone_count();

private:
	virtual void loadModel(const std::string_view& path) override;
};

