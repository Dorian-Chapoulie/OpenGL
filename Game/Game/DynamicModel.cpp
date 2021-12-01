#include "DynamicModel.h"

DynamicModel::DynamicModel(const std::string_view& path, const glm::vec3& position, float weight, glm::vec3 scale = glm::vec3(1.0f))
	: SkeletalModel(path, position, scale)
{
	if (weight < 0.1f) throw std::exception("The weight of a DynamicBody cannot be less that 0.1kg.");
	weight_ = weight;
	animation_ = new Animation(path, this);
	hit_boxes_.emplace_back(new HitBox(this));
}

DynamicModel::~DynamicModel()
{
}

std::vector<HitBox*>& DynamicModel::get_hit_boxes()
{
	return hit_boxes_;
}

Animation* DynamicModel::get_animation()
{
	return animation_;
}
