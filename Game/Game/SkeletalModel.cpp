#include "SkeletalModel.h"

SkeletalModel::SkeletalModel(
	const std::string& path,
	const glm::vec3& position,
	float weight,
	bool hasHitbox,
	bool hasMultipleHitboxes, glm::vec3 scale)
	: Model(path, position, weight, hasHitbox, hasMultipleHitboxes, scale)
{

}
