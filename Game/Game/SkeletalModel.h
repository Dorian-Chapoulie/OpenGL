#pragma once
#include "Model.h"
class SkeletalModel :
	public Model
{
	SkeletalModel(
		const std::string& path,
		const glm::vec3& position,
		float weight, bool hasHitbox,
		bool hasMultipleHitboxes, glm::vec3 scale
	);
};

