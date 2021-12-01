#pragma once
#include "Animation.h"
#include "HitBox.h"
#include "SkeletalModel.h"

class DynamicModel : public SkeletalModel
{
public:
	DynamicModel(const std::string_view& path, const glm::vec3& position, float weight, glm::vec3 scale);
	~DynamicModel();

	[[nodiscard]] std::vector<HitBox*>& get_hit_boxes();
	[[nodiscard]] Animation* get_animation();

private:
	std::vector<HitBox*> hit_boxes_;
	Animation* animation_;
	float weight_ = 1.0f;
};

