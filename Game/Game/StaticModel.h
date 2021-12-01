#pragma once
#include "HitBox.h"
#include "Model.h"

class StaticModel : public Model
{
public:
	StaticModel(const std::string_view& path, const glm::vec3& position, glm::vec3 scale = glm::vec3(1.0f));
	~StaticModel();

	[[nodiscard]] std::vector<HitBox*>& get_hit_boxes();

private:
	std::vector<HitBox*> hit_boxes_;
	const float weight_ = 0.0f;
};

